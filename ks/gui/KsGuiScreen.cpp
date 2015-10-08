/*
   Copyright (C) 2015 Preet Desai (preet.desai@gmail.com)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#include <ks/gui/KsGuiScreen.hpp>

namespace ks
{
    namespace gui
    {
        Screen::Screen(std::string name,
                       Orientation orientation,
                       std::pair<uint,uint> size_px,
                       std::pair<uint,uint> size_mm,
                       float dpi) :
                    name(name),
                    orientation(orientation),
                    size_px(size_px),
                    size_mm(size_mm),
                    dpi(dpi)
        {}
    }
}
