#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>
#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>

typedef struct _MyTransform {
    GstBaseTransform parent;
} MyTransform;

typedef struct _MyTransformClass {
    GstBaseTransformClass parent_class;
} MyTransformClass;

#define MY_TYPE_TRANSFORM (my_transform_get_type())
G_DECLARE_FINAL_TYPE(MyTransform, my_transform, MY, TRANSFORM, GstBaseTransform)

static GstFlowReturn my_transform_transform_ip(GstBaseTransform *base, GstBuffer *buf) {
    MyTransform *self = MY_TRANSFORM(base);

    GstMapInfo map;
    gst_buffer_map(buf, &map, GST_MAP_READWRITE);

    GstVideoFrame frame;
    GstVideoInfo info;

    if (!gst_video_frame_map(&frame, &info, buf, GST_MAP_READWRITE)) {
        gst_buffer_unmap(buf, &map);
        return GST_FLOW_ERROR;
    }

    int width = GST_VIDEO_INFO_WIDTH(&info);
    int height = GST_VIDEO_INFO_HEIGHT(&info);
    int stride = GST_VIDEO_FRAME_PLANE_STRIDE(&frame, 0);
    guint8 *data = GST_VIDEO_FRAME_PLANE_DATA(&frame, 0);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            data[y * stride + x * 3 + 0] = 255 - data[y * stride + x * 3 + 0]; // Invert red
            data[y * stride + x * 3 + 1] = 255 - data[y * stride + x * 3 + 1]; // Invert green
            data[y * stride + x * 3 + 2] = 255 - data[y * stride + x * 3 + 2]; // Invert blue
        }
    }

    gst_video_frame_unmap(&frame);
    gst_buffer_unmap(buf, &map);

    return GST_FLOW_OK;
}

static void my_transform_class_init(MyTransformClass *klass) {
    GstBaseTransformClass *base_transform_class = GST_BASE_TRANSFORM_CLASS(klass);
    base_transform_class->transform_ip = GST_DEBUG_FUNCPTR(my_transform_transform_ip);
}

static void my_transform_init(MyTransform *self) {
    // Initialize custom properties or variables
}

static gboolean my_transform_plugin_init(GstPlugin *plugin) {
    return gst_element_register(plugin, "mytransform", GST_RANK_NONE, MY_TYPE_TRANSFORM);
}

GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    mytransform,
    "My Transform Plugin",
    my_transform_plugin_init,
    VERSION,
    "LGPL",
    "GStreamer",
    "https://gstreamer.freedesktop.org/"
)
