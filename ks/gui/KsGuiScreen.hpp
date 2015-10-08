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

#ifndef KS_GUI_SCREEN_HPP
#define KS_GUI_SCREEN_HPP

#include <ks/shared/KsProperty.hpp>

namespace ks
{
    namespace gui
    {
        class Screen final
        {
        public:
            enum class Orientation {
                Landscape,
                Portrait,
                InvertedLandscape,
                InvertedPortrait
            };

            Screen(std::string name,
                   Orientation orientation,
                   std::pair<uint,uint> size_px,
                   std::pair<uint,uint> size_mm,
                   float dpi);

            ~Screen() = default;

            // Properties
            Property<std::string> name;
            Property<Orientation> orientation;
            Property<std::pair<uint,uint>> size_px;
            Property<std::pair<uint,uint>> size_mm;
            Property<float> dpi;
        };
    }
}

#endif // KS_GUI_SCREEN_HPP
