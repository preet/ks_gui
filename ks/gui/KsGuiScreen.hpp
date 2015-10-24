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
        // ============================================================= //

        class InvalidScreenRotation : public ks::Exception
        {
        public:
            InvalidScreenRotation(std::string msg);
            ~InvalidScreenRotation() = default;
        };

        // ============================================================= //

        class Screen final
        {
        public:
            using Size = std::pair<uint,uint>;

            enum class Rotation
            {
                CW_0=0,
                CW_90=90,
                CW_180=180,
                CW_270=270
            };

            Screen(std::string name,
                   Rotation rotation,
                   uint width_px,
                   uint height_px,
                   float xdpi,
                   float ydpi);

            ~Screen() = default;

            static Rotation ConvertRotation(uint rotation_degs);

            // Properties
            Property<std::string> name;
            Property<Rotation> rotation;
            Property<Size> size_px;
            Property<float> xdpi;
            Property<float> ydpi;
        };

        // ============================================================= //
    }
}

#endif // KS_GUI_SCREEN_HPP
