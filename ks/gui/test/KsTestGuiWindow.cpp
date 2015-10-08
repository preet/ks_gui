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

#include <ks/gui/KsGuiWindow.hpp>
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

    gui::Window::Attributes win_attribs;
    gui::Window::Properties win_props;

    // Create window using the application's EventLoop
    shared_ptr<gui::Window> win0 =
            app->CreateWindow(
                app->GetEventLoop(),
                win_attribs,
                win_props);

    // Create window using a dedicated render thread
    shared_ptr<EventLoop> win1_render_evl =
            make_shared<EventLoop>();

    std::thread win1_render_thread =
            EventLoop::LaunchInThread(win1_render_evl);

    shared_ptr<gui::Window> win1 =
            app->CreateWindow(
                win1_render_evl,
                win_attribs,
                win_props);

    (void)win0;
    (void)win1;

    // Run!
    app->Run();

    EventLoop::RemoveFromThread(win1_render_evl,
                                win1_render_thread,
                                true);

    return 0;
}
