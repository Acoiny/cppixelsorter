# Todo's
## UI
- [ ] finish checkbox's look
- [x] add margin into baseelement and make it set-able
- [ ] Enable saving of only selected (zoomed in) region
- [x] Make extra basic texturerect class (for hue-bar)
- [ ] adjust button sizes, maybe absolute?
- [ ] add textures for buttons and hue slider
- [ ] add dropdown picker
- [ ] add editable text box (for hue)
- [x] Cursor styles
- [ ] inotify for theme config

## Features
- [x] Add zooming functionality
- [x] make file-picker async
- [ ] sorting for other directions (after dropdown)
- [ ] save sorting (save the "spans") of one image and apply to different image

## Crazier features
- [ ] drawing on the image with the mouse :O


# Idea for UI refactor
Specific actions make objects "capture" the mouse state. This means, that they
get all mouse events FIRST. E.g. when holding a dropdown menu and moving the
mouse around, this menu gets all coming events before all other ui elements.

Other approach: Only "captured" element gets mouse events until it releases the
cursor again. The dropdown menu could work like this, it would capture the
mouse when you click it and then only it would get all mouse events, allowing
selection of all it's options. When you move the mouse away from it and click,
it would still "handle" the event but not act on anything and the mouse pointer
would be released.

`HandleMouseEvent` returns a tuple of `EventResult`, which indicates if the
event hasn't been handled yet, has been handled or has been handled _with a
focus capture_, and an `std::optional<std::shared_ptr<BaseElement>>` which
contains the element to capture, if the latter is the case.
If an element want's to remove itself from caputre, it just sends an empty
optional with the capture `EventResult`.

**Idea**: Maybe make a new subclass of `BaseElement` which is capturable?
Then the optional could contain that.
But why? Cleaner? Benefits?
