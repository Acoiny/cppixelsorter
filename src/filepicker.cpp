#include "filepicker.hpp"

#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
  int done;
  const char *request_path;
} FilterData;

static bool g_success = false;
static Filepicker *g_handle = nullptr;

DBusHandlerResult response_filter(DBusConnection *conn, DBusMessage *message,
                                  void *user_data)
{
  (void)conn;
  FilterData *data = (FilterData *)user_data;

  if (!dbus_message_is_signal(message, "org.freedesktop.portal.Request",
                              "Response"))
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  if (strcmp(dbus_message_get_path(message), data->request_path) != 0)
    return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

  DBusMessageIter sig_iter;
  dbus_message_iter_init(message, &sig_iter);

  dbus_uint32_t response;
  dbus_message_iter_get_basic(&sig_iter, &response);

  if (response == 0)
  {
    dbus_message_iter_next(&sig_iter);
    DBusMessageIter results_iter;
    dbus_message_iter_recurse(&sig_iter, &results_iter);

    while (dbus_message_iter_get_arg_type(&results_iter) ==
           DBUS_TYPE_DICT_ENTRY)
    {
      DBusMessageIter entry_iter, variant_iter;
      const char *key;

      dbus_message_iter_recurse(&results_iter, &entry_iter);
      dbus_message_iter_get_basic(&entry_iter, &key);

      if (strcmp(key, "uris") == 0)
      {
        dbus_message_iter_next(&entry_iter);
        dbus_message_iter_recurse(&entry_iter, &variant_iter);

        DBusMessageIter array_iter;
        dbus_message_iter_recurse(&variant_iter, &array_iter);

        while (dbus_message_iter_get_arg_type(&array_iter) == DBUS_TYPE_STRING)
        {
          const char *uri;
          dbus_message_iter_get_basic(&array_iter, &uri);
          printf("Selected file: %s\n", uri);
          if (g_handle)
          {
            std::string str(uri);
            if (str.starts_with("file://"))
            {
              str = str.substr(7);
            }
            g_handle->m_filenames.push_back(str);
            g_success = true;
          }
          dbus_message_iter_next(&array_iter);
        }
        break;
      }
      dbus_message_iter_next(&results_iter);
    }
  }
  else if (response == 1)
  {
    printf("File selection cancelled.\n");
  }
  else
  {
    printf("File selection failed (response: %u).\n", response);
  }

  data->done = 1;
  return DBUS_HANDLER_RESULT_HANDLED;
}

bool Filepicker::open(bool saving)
{
  g_success = false;

  DBusConnection *conn;
  DBusError err;
  DBusMessage *msg, *reply;
  DBusMessageIter args, dict;

  dbus_error_init(&err);

  conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
  if (dbus_error_is_set(&err))
  {
    fprintf(stderr, "Connection Error (%s)\n", err.message);
    dbus_error_free(&err);
    return false;
  }

  msg = dbus_message_new_method_call(
      "org.freedesktop.portal.Desktop", "/org/freedesktop/portal/desktop",
      "org.freedesktop.portal.FileChooser", saving ? "SaveFile" : "OpenFile");
  if (msg == NULL)
  {
    fprintf(stderr, "Message Null\n");
    dbus_connection_unref(conn);
    return false;
  }

  dbus_message_iter_init_append(msg, &args);

  const char *parent_window = "";
  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &parent_window);

  const char *title = "Select a File";
  dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &title);

  dbus_message_iter_open_container(&args, DBUS_TYPE_ARRAY, "{sv}", &dict);
  dbus_message_iter_close_container(&args, &dict);

  reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);
  dbus_message_unref(msg);

  if (dbus_error_is_set(&err))
  {
    fprintf(stderr, "Error sending message (%s)\n", err.message);
    dbus_error_free(&err);
    dbus_connection_unref(conn);
    return false;
  }

  const char *request_path;
  if (!dbus_message_get_args(reply, &err, DBUS_TYPE_OBJECT_PATH, &request_path,
                             DBUS_TYPE_INVALID))
  {
    fprintf(stderr, "Error getting request path (%s)\n", err.message);
    dbus_error_free(&err);
    dbus_message_unref(reply);
    dbus_connection_unref(conn);
    return false;
  }

  // Copy the path since reply will be freed
  char *req_path = strdup(request_path);
  dbus_message_unref(reply);

  char match_rule[512];
  snprintf(match_rule, sizeof(match_rule),
           "type='signal',interface='org.freedesktop.portal.Request',"
           "member='Response',path='%s'",
           req_path);

  dbus_bus_add_match(conn, match_rule, &err);
  dbus_connection_flush(conn);
  if (dbus_error_is_set(&err))
  {
    fprintf(stderr, "Match Error (%s)\n", err.message);
    dbus_error_free(&err);
    free(req_path);
    dbus_connection_unref(conn);
    return false;
  }

  // Install a filter to catch the Response signal during dispatch
  FilterData filter_data = {.done = 0, .request_path = req_path};
  dbus_connection_add_filter(conn, response_filter, &filter_data, NULL);
  g_handle = this;

  // Dispatch loop — read_write_dispatch handles internal messages properly
  while (!filter_data.done)
  {
    dbus_connection_read_write_dispatch(conn, -1);
  }

  dbus_connection_remove_filter(conn, response_filter, &filter_data);
  g_handle = nullptr;

  dbus_bus_remove_match(conn, match_rule, NULL);
  free(req_path);
  dbus_connection_unref(conn);

  return g_success;
}
