/*
                    The HumbleHacker Keyboard Project
                 Copyright © 2008-2010, David Whetstone
               david DOT whetstone AT humblehacker DOT com

  This file is a part of The HumbleHacker Keyboard Project.

  The HumbleHacker Keyboard Project is free software: you can redistribute
  it and/or modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  The HumbleHacker Keyboard Project is distributed in the hope that it will
  be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License along
  with The HumbleHacker Keyboard Project.  If not, see
  <http://www.gnu.org/licenses/>.

*/

#include "binding.h"

/*
 *    KeyBinding
 */

void
KeyBinding__copy(const KeyBinding *this, KeyBinding *dst)
{
  dst->kind    = this->kind;
  dst->premods = this->premods;
  dst->target  = this->target;
}

const ModeTarget*
KeyBinding__get_mode_target(const KeyBinding *this)
{
  static ModeTarget target;
  memcpy_P((void*)&target, (PGM_VOID_P)this->target, sizeof(ModeTarget));
  return &target;
}

const MacroTarget*
KeyBinding__get_macro_target(const KeyBinding *this)
{
  static MacroTarget target;
  memcpy_P((void*)&target, (PGM_VOID_P)this->target, sizeof(MacroTarget));
  return &target;
}

const MapTarget*
KeyBinding__get_map_target(const KeyBinding *this)
{
  static MapTarget target;
  memcpy_P((void*)&target, (PGM_VOID_P)this->target, sizeof(MapTarget));
  return &target;
}

/*
 *    KeyBindingArray
 */

const KeyBinding*
KeyBindingArray__get_binding(const KeyBindingArray *this, uint8_t index)
{
  static KeyBinding binding;
  memcpy_P((void*)&binding, (PGM_VOID_P)&this->data[index], sizeof(KeyBinding));
  return &binding;
}

/*
 *    MacroTarget
 */

const MapTarget*
MacroTarget__get_map_target(const MacroTarget *this, uint8_t index)
{
  static MapTarget target;
  memcpy_P((void*)&target, (PGM_VOID_P)&this->targets[index], sizeof(MapTarget));
  return &target;
}

/*
 *    All Bindings
 */

<% for i,keymap in ipairs(kb.keymaps) do
     for location,key in pairs(keymap.keys) do
       for i,binding in ipairs(key.bindings) do
         ident = binding_identifier(keymap, key.location, binding.premods, binding.class)
         if binding.class == 'Map' then %>
const MapTarget <%=ident%> PROGMEM = { <%= binding.modifiers %>, HID_USAGE_<%= normalize_identifier(binding.usage.name) %> };<%
         elseif binding.class == 'Macro' then %>
const MapTarget <%=ident%>Targets[] PROGMEM =
{
<%         for i,map in ipairs(binding.maps) do %>
  { <%= map.modifiers %>, HID_USAGE_<%= normalize_identifier(map.usage.name) %> },
<%         end %>
};

const MacroTarget <%= ident %> PROGMEM = { <%= #binding.maps %>, &<%= ident %>Targets[0] }; <%
         elseif binding.class == 'Mode' then %>
const ModeTarget <%= ident %> PROGMEM = { <%= string.upper(binding.class) %>, kbd_map_<%= binding.name %>_mx }; <%
         else
           %><%="/* What? */"%><%
         end
       end
     end
   end
%>

/*
 *    Aggregated bindings per key
 */

<% for i,keymap in ipairs(kb.keymaps) do
     for location,key in pairs(keymap.keys) do %>
const KeyBinding <%= keymap.name %>_<%= key.location %>[] PROGMEM =
{<%    for i,binding in ipairs(key.bindings) do %>
  { <%   if binding.class == 'Map' then
    %>MAP, <%
         elseif binding.class == 'Macro' then
    %>MACRO, <%
         elseif binding.class == 'Mode' then
    %>MODE, <%
         else
    %>/* What's a <%= binding.class %>? */ NULL, <%
         end
%><%= binding.premods %>, (void*)&<%= binding_identifier(keymap, key.location, binding.premods, binding.class) %> }, <%
       end %>
};
<%
     end
   end
%>


