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
#include <ks/gl/KsGLCommands.hpp>

using namespace ks;

namespace {

    uint win_height;
    uint win_width;
    uint k;
    uint layer_height;
    uint layer_width;


    class RenderLayer : public ks::Object
    {
    public:
        using base_type = ks::Object;

        RenderLayer(ks::Object::Key const &key,
                    shared_ptr<EventLoop> evl,
                    shared_ptr<gui::Layer> layer) :
            ks::Object(key,evl),
            m_layer(layer)
        {

        }

        void Init(ks::Object::Key const &,
                  shared_ptr<RenderLayer> const &this_layer)
        {
            m_layer->signal_render.Connect(
                        this_layer,
                        &RenderLayer::OnRender,
                        ks::ConnectionType::Direct);
        }

        ~RenderLayer() = default;

        virtual void OnRender() = 0;

    private:
        shared_ptr<gui::Layer> m_layer;
    };


    class RedLayer : public RenderLayer
    {
    public:
        using base_type = RenderLayer;

        RedLayer(ks::Object::Key const &key,
                 shared_ptr<EventLoop> evl,
                 shared_ptr<gui::Layer> layer) :
            RenderLayer(key,evl,layer)
        {}

        void Init(ks::Object::Key const &,
                  shared_ptr<RedLayer> const &)
        {}

        void OnRender()
        {
            glDisable(GL_SCISSOR_TEST);
            glClearColor(0,0,0,1);
            glClear(gl::ColorBufferBit);

            glEnable(GL_SCISSOR_TEST);
            glScissor(k,win_height-layer_height-k,layer_width,layer_height);
            glClearColor(1,0,0,1);
            gl::Clear(gl::ColorBufferBit);
        }
    };


    class GreenLayer : public RenderLayer
    {
    public:
        using base_type = RenderLayer;

        GreenLayer(ks::Object::Key const &key,
                 shared_ptr<EventLoop> evl,
                 shared_ptr<gui::Layer> layer) :
            RenderLayer(key,evl,layer)
        {}

        void Init(ks::Object::Key const &,
                  shared_ptr<GreenLayer> const &)
        {}

        void OnRender()
        {
            glEnable(GL_SCISSOR_TEST);
            glScissor(k*2,win_height-layer_height-k*2,layer_width,layer_height);
            glClearColor(0,1,0,1);
            gl::Clear(gl::ColorBufferBit);
        }
    };


    class BlueLayer : public RenderLayer
    {
    public:
        using base_type = RenderLayer;

        BlueLayer(ks::Object::Key const &key,
                 shared_ptr<EventLoop> evl,
                 shared_ptr<gui::Layer> layer) :
            RenderLayer(key,evl,layer)
        {}

        void Init(ks::Object::Key const &,
                  shared_ptr<BlueLayer> const &)
        {}

        void OnRender()
        {
            glEnable(GL_SCISSOR_TEST);
            glScissor(k*3,win_height-layer_height-k*3,layer_width,layer_height);
            glClearColor(0,0,1,1);
            gl::Clear(gl::ColorBufferBit);
        }
    };
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    // Create application
    shared_ptr<gui::Application> app =
            make_object<gui::Application>();


    // Create window
    gui::Window::Attributes win_attribs;
    gui::Window::Properties win_props;

    shared_ptr<gui::Window> window =
            app->CreateWindow(
                app->GetEventLoop(),
                win_attribs,
                win_props);


    // Create Layers
    win_width = window->size.Get().first;
    win_height = window->size.Get().second;

    k = win_height/10;
    layer_height = win_height-4*k;
    layer_width = win_width-4*k;

    // first layer
    auto layer_red = window->CreateLayer(0);
    auto render_layer_red =
            make_object<RedLayer>(
                app->GetEventLoop(),
                layer_red);

    // second layer
    auto layer_green = window->CreateLayer(1);
    auto render_layer_green =
            make_object<GreenLayer>(
                app->GetEventLoop(),
                layer_green);

    // third layer
    auto layer_blue = window->CreateLayer(2);
    auto render_layer_blue =
            make_object<BlueLayer>(
                app->GetEventLoop(),
                layer_blue);

    (void)render_layer_red;
    (void)render_layer_green;
    (void)render_layer_blue;


    // Start rendering
    shared_ptr<CallbackTimer> timer =
            make_object<CallbackTimer>(
                window->GetEventLoop(),
                milliseconds(200),
                [window](){
                    window->Render();
                });

    timer->Start();


    // Run!
    app->Run();

    return 0;
}
