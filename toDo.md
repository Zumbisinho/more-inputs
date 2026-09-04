# Remmeber
SMJS Ícone de cargo, Index Staff (role) — 16:33
not with primitives, strings and vectors
primitives are trival to copy
strings you use std::string_view or ZStringView if you plan to c_str it and just copy the view
vectors you std::span and just copy the span

# To be Added

- ~~Mobile Button Port~~
- ~~Add a trigger that detect if the mod is loaded~~ *the object collab already does that*
- ~~Add a splash art from the icon to the Tab~~
- ~~Add a cache when selecting custom triggers~~ *new api for doing that*
- ~~Add a Automatic UI system for lazy ass people~~
- ~~Add a trigget to enable/disable a key (most for mobiles)~~ *almost that, just need to disable from pc*
- _Add a text to mobile button or image (enable hue, rot, offset, scale and wrap) moved to 2.1.0_
- ~~Add a way to convert old to new keys~~
- ~~Add the key config to the mobile button~~
- ~~Add way to select what button you wanna edit on editmobilebuttons~~
- ~~Add a enable/disable Snap button~~
- _Add a side menu to config text label, size and input a image moved to 2.1.0_
- ~~Add the mobile preview to the level editor thing~~
- ~~Add a keyboard to mobile player add the key -- ui done just need to implement only on mobile  #ifdef GEODE_IS_MOBILE~~
- ~~Add the edit local mobile buttons~~
- ~~hook the EditorUI::editObject to temp remove the aux triggers from the m_selectedObjects soo the ObjectCollabApi only process the customtriggers, and right after realocate the objs to the m_selectedObjs~~
- ~~every time you select a macro or a aux trigger, it selects all (link control or something);~~



# To be Fixed

- ~~Clicking on Touch Macro with anything selected~~
- ~~Not being able to input after adding keys to the level~~
- ~~Action label not having a limit to chars~~
- ~~You can create actions without keys~~
- ~~Adding a key out of order wipes out the remaing keys.~~
- ~~Swithing with a old level to a new one fucks touch trigger UI | *touchmacro.hpp ~171*~~ 
- ~~Removing the last key left crashes the game.~~
- ~~Saving the level sometimes ruins the keybind mobile layout~~
- Remake the mobile button to a good version

- ~~fix it to be playable on my pohne~~
- add a system that shows a popup when versions is outdated
- ~~add the new params to the saving system to not fuck up my next month~~
- ~~1 time the nextFreecontrolid got a panick and start to overflowing my ram, add a limit ig idk -- tung tung sahurcles and its a garbage problem with accessing non controled id game objects~~ *just .isTrigger() was missing*
- ~~free control ids when deleting macros.~~
- ~~duplicating a macro needs to changes its control id~~
- ~~keybind togle needs info rewrite and activate gourp -> activate keybind~~

# To be Refactored

- ~~Keybinds saving System~~
- ~~Custom triggers~~ *object collab + macro trigger = holy coding*



