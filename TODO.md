# Todo's
## UI
- [x] add margin into baseelement and make it set-able
- [ ] Enable saving of only selected (zoomed in) region
- [x] Make extra basic texturerect class (for hue-bar)
- [ ] adjust button sizes, maybe absolute?
- [ ] add textures for buttons and hue slider
- [ ] finish checkbox's look
- [ ] add dropdown picker
- [ ] add editable text box (for hue)
- [x] Cursor styles

## Features
- [x] Add zooming functionality
- [x] make file-picker async
- [ ] sorting for other directions (after dropdown)

## Crazier features
- [ ] drawing on the image with the mouse :O


# Idea for UI refactor
Specific actions make objects "capture" the mouse state. This means, that they
get all mouse events FIRST. E.g. when holding a button and moving the mouse
around, the button gets all coming events before all other ui elements.

Other approach: Only "captured" element gets mouse events until it releases the
cursor again. The dropdown menu could work like this, it would capture the
mouse when you click it and then only it would get all mouse events, allowing
selection of all it's options. When you move the mouse away from it and click,
it would still "handle" the event but not act on anything and the mouse pointer
would be released.

`HandleMouseEvent` would have to get a different return value. It would still
have to indicate if the event was handled like it does already but it would
also need to pass up the element that captured the event so the `UiManager` can
get a hold of it and pass all future events to it. Probably a tuple like
`std::pair<bool, std::optional<std::shared_ptr<BaseElement>>>`.
The boolean would function like it has until now but the optional pointer
would be capture if it was set.
