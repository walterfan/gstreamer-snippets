#include "gtest/gtest.h"
#include "glib-object.h"
#include "time.h"
#include <stdint.h>
#include <string>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
using namespace testing;

class GObjectTest: public testing::Test
{
public:
    GObjectTest()
    {
    }

    virtual ~GObjectTest() { }

    virtual void SetUp() {
        std::string strClassName(::testing::UnitTest::GetInstance()->current_test_info()->test_case_name());
        std::string strFuncName(::testing::UnitTest::GetInstance()->current_test_info()->name());
        m_strTestName = strClassName + "." + strFuncName;
    }
    virtual void TearDown()  {

    }
protected:
    std::string m_strTestName;
};


TEST_F(GObjectTest, set_get_property)
{
    std::cout << "gobject properties..." << std::endl;
    GObject *instance1, *instance2;
    GObjectClass *class1, *class2;
    
    instance1 = (GObject *)g_object_new (G_TYPE_OBJECT, NULL);
    instance2 = (GObject *)g_object_new (G_TYPE_OBJECT, NULL);
    g_print ("The address of instance1 is %p\n", instance1);
    g_print ("The address of instance2 is %p\n", instance2);
 
    class1 = G_OBJECT_GET_CLASS (instance1);
    class2 = G_OBJECT_GET_CLASS (instance2);
    g_print ("The address of the class of instance1 is %p\n", class1);
    g_print ("The address of the class of instance2 is %p\n", class2);
 
    g_object_unref (instance1);
    g_object_unref (instance2);
  
}
//also can check the number of instance of a type with g_type_instance_count
//note:  export GOBJECT_DEBUG=instance-count
TEST_F(GObjectTest, check_ref_count)
{
    GObject * obj1 = (GObject *)g_object_new (G_TYPE_OBJECT, NULL);

    g_assert_nonnull (obj1);

    // the contents of the pointer are reset to NULL when the last reference to
    // the GObject instance goes away; by passing a pointer to the same instance
    // we can check it for NULL later
    g_object_add_weak_pointer (G_OBJECT (obj1), (gpointer *) &obj1);

    // ...test FooObject...

    // drop the last reference
    g_object_unref (obj1);

    // at this point, the object should be NULL if nothing is holding
    // an additional reference.
    g_assert_null (obj1);

}

