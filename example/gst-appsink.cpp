#include <stdio.h>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <opencv2/opencv.hpp>
#include <iostream>


static gboolean on_message(GstBus *bus, GstMessage *message, gpointer user_data) {
    GMainLoop *loop = (GMainLoop *)user_data;
    switch (GST_MESSAGE_TYPE(message)) {
        case GST_MESSAGE_EOS:
            std::cout << "End of stream" << std::endl;
            g_main_loop_quit(loop);
            break;
        case GST_MESSAGE_ERROR: {
            GError *err;
            gchar *debug_info;
            gst_message_parse_error(message, &err, &debug_info);
            std::cout << "Error: " << err->message << std::endl;
            g_clear_error(&err);
            g_free(debug_info);
            g_main_loop_quit(loop);
            break;
        }
        default:
            break;
    }
    return TRUE;
}

static GstFlowReturn new_sample_callback(GstAppSink *sink, gpointer user_data) {
    GstSample *sample = gst_app_sink_pull_sample(sink);
    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstCaps *caps = gst_sample_get_caps(sample);

    GstMapInfo map;
    gst_buffer_map(buffer, &map, GST_MAP_READ);

    GstStructure *structure = gst_caps_get_structure(caps, 0);
    int width, height;
    gst_structure_get_int(structure, "width", &width);
    gst_structure_get_int(structure, "height", &height);
    std::cout << "width=" << width << ", height=" << height << std::endl;
    /*
    cv::Mat frame(height, width, CV_8UC3, (char*)map.data);
    cv::imshow("Frame", frame);
    if (cv::waitKey(1) == 'q') {
        gst_buffer_unmap(buffer, &map);
        gst_sample_unref(sample);
        return GST_FLOW_EOS;
    }
    */
    gst_buffer_unmap(buffer, &map);
    gst_sample_unref(sample);
    return GST_FLOW_OK;
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <xxx.mp4>" << std::endl;
        return EXIT_FAILURE;
    }
    char buffer[200];
    snprintf(buffer, sizeof(buffer), "filesrc location=\"%s\" ! decodebin ! videoconvert ! appsink name=sink", argv[1]);
    std::cout << "pipeline is " << buffer << std::endl;
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    GstElement *pipeline = gst_parse_launch(buffer, NULL);
    GstElement *sink = gst_bin_get_by_name(GST_BIN(pipeline), "sink");

    g_object_set(sink, "emit-signals", TRUE, "sync", FALSE, NULL);
    g_signal_connect(sink, "new-sample", G_CALLBACK(new_sample_callback), NULL);

    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, on_message, loop);
    gst_object_unref(bus);

    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    g_main_loop_run(loop);

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);

    return 0;
}
