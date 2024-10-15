#include "gtest/gtest.h"
#include "string_util.h"
#include "time_util.h"
#include "time.h"
#include <stdint.h>
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <gst/gst.h>
using namespace std;
using namespace testing;

TEST(GstUtilTest, refcount1)
{
    gst_init(NULL, NULL);
    g_print("will create fakesink element\n");
    GstElement *element = gst_element_factory_make("fakesink", "sink");

    g_print("Ref count incremented\n");
    gst_object_ref(element);  // Explicitly increase reference count

    g_print("Ref count decremented\n");
    gst_object_unref(element); // Decrease reference count

    g_print("Ref count decremented\n");
    gst_object_unref(element); // Final unref, object should be destroyed
    //gst_object_unref(element); //---error
}

TEST(GstUtilTest, refcount2)
{
    gst_init(NULL, NULL);
    GstElement *pipeline = gst_pipeline_new("test-pipeline");
    GstElement *element = gst_element_factory_make("fakesink", "sink");

    //gst_bin_add does not add ref count, just take ownership
    gst_bin_add(GST_BIN(pipeline), element);
    //gst_bin_remove does ref count decreasement
    gst_bin_remove(GST_BIN(pipeline), element); //ref-count==0 of element
    //gst_object_unref(element);
    gst_object_unref(pipeline);
}
