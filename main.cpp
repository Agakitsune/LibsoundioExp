/*
** EPITECH PROJECT, 2023
** LibsoundioExp
** File description:
** main
*/

#include <exception>
#include <vector>
#include <functional>
#include <memory>
#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <soundio/soundio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "node/io/IO.hpp"
#include "node/io/Field.hpp"
#include "node/Wave.hpp"
#include "node/Mix.hpp"
#include "node/Output.hpp"
#include "node/Micro.hpp"

SoundIo *setup() {
    int err;
    SoundIo *soundio = soundio_create();
    if (!soundio) {
        fprintf(stderr, "out of memory\n");
        return nullptr;
    }

    if ((err = soundio_connect(soundio))) {
        fprintf(stderr, "error connecting: %s", soundio_strerror(err));
        return nullptr;
    }

    soundio_flush_events(soundio);

    return soundio;
}

SoundIoDevice *setupDevice(SoundIo *soundio) {
    int default_out_device_index = soundio_default_output_device_index(soundio);
    if (default_out_device_index < 0) {
        fprintf(stderr, "no output device found");
        return nullptr;
    }

    SoundIoDevice *device = soundio_get_output_device(soundio, default_out_device_index);
    if (!device) {
        fprintf(stderr, "out of memory");
        return nullptr;
    }

    return device;
}

SoundIoDevice *setupInputDevice(SoundIo *soundio) {
    int default_in_device_index = soundio_default_input_device_index(soundio);
    if (default_in_device_index < 0) {
        fprintf(stderr, "no input device found");
        return nullptr;
    }

    SoundIoDevice *device = soundio_get_input_device(soundio, default_in_device_index);
    if (!device) {
        fprintf(stderr, "out of memory");
        return nullptr;
    }

    return device;
}

static float seconds_offset = 0.0f;

static SoundIo *io;

static synthetizer::Output output;
static std::vector<std::shared_ptr<synthetizer::Node>> nodes;
static std::vector<std::shared_ptr<synthetizer::Micro>> micros;

void write_callback(struct SoundIoOutStream *outstream,
        int frame_count_min, int frame_count_max)
{
    const struct SoundIoChannelLayout *layout = &outstream->layout;
    float float_sample_rate = outstream->sample_rate;
    float seconds_per_frame = 1.0f / float_sample_rate;
    struct SoundIoChannelArea *areas;
    int frames_left = frame_count_max;
    int err;

    while (frames_left > 0) {
        int frame_count = frames_left;

        if ((err = soundio_outstream_begin_write(outstream, &areas, &frame_count))) {
            fprintf(stderr, "%s\n", soundio_strerror(err));
            exit(1);
        }

        if (!frame_count)
            break;

        std::vector<float> frames;
        std::vector<float> rframes;
        for (int frame = 0; frame < frame_count; ++frame) {
            float value = seconds_offset + frame * seconds_per_frame;
            frames.push_back(value);
            if (rframes.size() < 128)
                rframes.push_back(value);
        }

        for (auto &node : nodes)
            node->renderFrames(rframes);

        output.inputFrames(frames);
        synthetizer::Sample *sample = output.getSample();

        for (int frame = 0; frame < frame_count; ++frame) {
            for (int channel = 0; channel < layout->channel_count; ++channel) {
                float *ptr = (float*)(areas[channel].ptr + areas[channel].step * frame);
                if (sample != nullptr)
                    *ptr = sample->at(frame);
                else
                    *ptr = 0.0f;
            }
        }

        seconds_offset = fmodf(seconds_offset +
            seconds_per_frame * frame_count, 1.0f);

        if ((err = soundio_outstream_end_write(outstream))) {
            fprintf(stderr, "%s\n", soundio_strerror(err));
            exit(1);
        }

        frames_left -= frame_count;
    }
}

void read_callback(SoundIoInStream *instream, int frame_count_min, int frame_count_max) {
    const struct SoundIoChannelLayout *layout = &instream->layout;
    struct SoundIoChannelArea *areas;
    int frames_left = frame_count_max;
    int err;

    while (frames_left > 0) {
        int frame_count = frames_left;

        if (err = soundio_instream_begin_read(instream, &areas, &frame_count)) {
            fprintf(stderr, "%s\n", soundio_strerror(err));
            exit(1);
        }

        if (!frame_count)
            break;

        std::vector<float> input;
        for (int frame = 0; frame < frame_count; ++frame) {
            for (int channel = 0; channel < layout->channel_count; ++channel) {
                float *ptr = (float*)(areas[channel].ptr + areas[channel].step * frame);
                input.push_back(*ptr);
            }
        }

        for (auto &micro : micros)
            micro->feedInput(input);

        if (err = soundio_instream_end_read(instream)) {
            fprintf(stderr, "%s\n", soundio_strerror(err));
            exit(1);
        }

        frames_left -= frame_count;
    }
}

SoundIoOutStream *setupStream(SoundIoDevice *device) {
    int err;
    SoundIoOutStream *outstream = soundio_outstream_create(device);
    outstream->format = SoundIoFormatFloat32NE;
    outstream->write_callback = write_callback;

    if ((err = soundio_outstream_open(outstream))) {
        fprintf(stderr, "unable to open device: %s\n", soundio_strerror(err));
        return nullptr;
    }

    if (outstream->layout_error)
        fprintf(stderr, "unable to set channel layout: %s\n", soundio_strerror(outstream->layout_error));

    if ((err = soundio_outstream_start(outstream))) {
        fprintf(stderr, "unable to start device: %s\n", soundio_strerror(err));
        return nullptr;
    }

    return outstream;
}

SoundIoInStream *setupMicro(SoundIoDevice *device) {
    int err;
    SoundIoInStream *instream = soundio_instream_create(device);
    instream->format = SoundIoFormatFloat32NE;
    instream->read_callback = read_callback;

    if (err = soundio_instream_open(instream)) {
        fprintf(stderr, "unable to open device: %s\n", soundio_strerror(err));
        return nullptr;
    }

    if (instream->layout_error)
        fprintf(stderr, "unable to set channel layout: %s\n", soundio_strerror(instream->layout_error));

    if ((err = soundio_instream_start(instream))) {
        fprintf(stderr, "unable to start device: %s\n", soundio_strerror(err));
        return nullptr;
    }

    return instream;
}

int main(int ac, char **av) {
    io = setup();
    if (!io)
        return 1;
    SoundIoDevice *device = setupDevice(io);
    if (!device)
        return 1;
    SoundIoDevice *inDevice = setupInputDevice(io);
    if (!device)
        return 1;
    SoundIoOutStream *stream = setupStream(device);
    if (!stream)
        return 1;
    SoundIoInStream *micro = setupMicro(inDevice);
    if (!micro)
        return 1;

    if (!glfwInit())
        return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *_handle = glfwCreateWindow(1000, 1000, "Synthetizer", nullptr, nullptr);
    if (!_handle)
        return 1;
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwMakeContextCurrent(_handle);
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
        return 1;

    glfwSwapInterval(1);

    // ImGui setup

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO &guiIo = ImGui::GetIO();
    guiIo.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(_handle, true);
    ImGui_ImplOpenGL3_Init();

    bool addMenu = false;
    ImVec2 addPos;

    while (!glfwWindowShouldClose(_handle)) {
        glfwPollEvents();

        if (glfwGetKey(_handle, GLFW_KEY_Q) == GLFW_PRESS) {
            addMenu = true;
            double x, y;
            glfwGetCursorPos(_handle, &x, &y);
            addPos = ImVec2(x, y);
        }

        soundio_flush_events(io);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::ShowDemoWindow();

        for (auto &node : nodes)
            node->render();

        output.render();

        if (addMenu) {
            ImGui::SetNextWindowPos(addPos);
            ImGui::SetNextWindowSize(ImVec2(100, -1));
            bool select = false;
            ImGui::Begin("Add", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
            {
                if (ImGui::Selectable("Wave", &select)) {
                    nodes.push_back(std::make_shared<synthetizer::Wave>());
                    addMenu = false;
                }
                if (ImGui::Selectable("Mix", &select)) {
                    nodes.push_back(std::make_shared<synthetizer::Mix>());
                    addMenu = false;
                }
                if (ImGui::Selectable("Micro", &select)) {
                    micros.push_back(std::make_shared<synthetizer::Micro>());
                    nodes.push_back(micros.back());
                    addMenu = false;
                }
                if (!ImGui::IsWindowFocused())
                    addMenu = false;
            }
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(_handle);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(_handle);

    soundio_instream_destroy(micro);
    soundio_outstream_destroy(stream);
    soundio_device_unref(device);
    soundio_destroy(io);
}
