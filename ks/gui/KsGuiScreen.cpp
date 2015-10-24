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
        InvalidScreenRotation::InvalidScreenRotation(std::string msg) :
            ks::Exception(ks::Exception::ErrorLevel::ERROR,
                          std::move(msg),true)
        {}

        Screen::Screen(std::string name,
                       Rotation rotation,
                       uint width_px,
                       uint height_px,
                       float xdpi,
                       float ydpi) :
            name(name),
            rotation(rotation),
            size_px(Size(width_px,height_px)),
            xdpi(xdpi),
            ydpi(ydpi)
        {}

        Screen::Rotation Screen::ConvertRotation(uint rotation_degs)
        {
            if(rotation_degs == 0) {
                return Rotation::CW_0;
            }
            else if(rotation_degs == 90) {
                return Rotation::CW_90;
            }
            else if(rotation_degs == 180) {
                return Rotation::CW_180;
            }
            else if(rotation_degs == 270){
                return Rotation::CW_270;
            }
            else
            {
                throw InvalidScreenRotation(
                            "Invalid Screen Rotation value: "+
                            ks::to_string(rotation_degs));
            }
        }
    }
}
