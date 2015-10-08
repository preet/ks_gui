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

#include <ks/gui/KsGuiApplication.hpp>
#include <ks/platform/KsPlatformMain.hpp>

using namespace ks;

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // Create application
    shared_ptr<gui::Application> app =
            make_object<gui::Application>();

    // Dump screen information
    auto list_screens = app->GetScreens();
    for(auto& screen : list_screens) {
        LOG.Trace() << "Screen: " << screen->name.Get();
        LOG.Trace() << "Orientation: "
                    << static_cast<uint>(screen->orientation.Get());
        LOG.Trace() << "Resolution: "
                    << screen->size_px.Get().first << ","
                    << screen->size_px.Get().second;
        LOG.Trace() << "Dimensions: "
                    << screen->size_mm.Get().first << ","
                    << screen->size_mm.Get().second;
        LOG.Trace() << "DPI: " << screen->dpi.Get();
    }


    return 0;
}
