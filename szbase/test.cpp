#include "gui/SDLApp.h"

// TEST
#include "test/TestFramework.h"
#include "ds/Delegate.h"

#include "gui/widget/UIFrame.h"
#include "gui/widget/UIButton.h"
#include "gui/layout/AnchorLayout.h"

namespace Test_Delegate
{
    #pragma warning( push )
    #pragma warning( disable : 26800 )

    using namespace sz_test;
    using namespace sz_ds;

    class Test
    {
    public:
        static int sValue;
        int value = 0;

        // 成员函数(void (Test*)(int))
        void member_method_void_int(int x)
        {
            log(LogLevel::INFO, "  [Invoked] Member Method (void (Test*)(int))");
            value += x;
        }

        // 成员函数(int (Test*)(int, int))
        int member_method_int_intAint(int a, int b)
        {
            log(LogLevel::INFO, "  [Invoked] Member Method (int (Test*)(int, int))");
            return a + b;
        }

        // 静态成员函数(static void (Test::*)(int))
        static void static_member_method_void_int(int x)
		{
            log(LogLevel::INFO, "  [Invoked] Static Member Method (static void (Test::*)(int))");
            sValue += x;
		}
    };
    int Test::sValue = 0;
    
    int sValue = 0;
    // 静态函数(static void (*)(int))
    static void static_function_void_int(int x)
    {
        log(LogLevel::INFO, "  [Invoked] Static Function (static void (*)(int))");
        sValue += x;
    }

    // 全局函数(int(*)(int, int))
    int global_function_int_intAint(int a, int b)
    {
        log(LogLevel::INFO, "  [Invoked] Global Function (int(*)(int, int))");
        return a * b;
    }

    // 委托状态打印
    template<typename R, typename... Args>
    void print_delegate_state(const std::string& name, const Delegate<R, Args...>& d)
    {
        std::ostringstream oss;
        oss << ">>> " << name << " state: " << (d ? "BOUND" : "UNBOUND")
            << ", Address: " << (const void*)&d;
        log(LogLevel::INFO, oss.str());
    }

    // 测试委托
	int Test_Delegate(int argc, char* argv[])
	{
        print_section("Test_Delegate");

        Test t1;
        Delegate<void, int> d_void_int;
        Delegate<int, int, int> d_int_intAint;
        
        // 测试默认状态
        print_subsection("Default state");
        print_delegate_state("d_void (initial)", d_void_int);
        TEST_ASSERT(!d_void_int, "Default constructed delegate should be UNBOUND");

        // 测试绑定成员函数
        print_subsection("Binding Member Method");
        d_void_int.Bind(&t1, &Test::member_method_void_int);
        print_delegate_state("d_void_int (after bind)", d_void_int);
        TEST_ASSERT(d_void_int, "Delegate should be BOUND after binding member");
        d_void_int(10);
        TEST_ASSERT(t1.value == 10, "Member function invocation");

        d_int_intAint.Bind(&t1, &Test::member_method_int_intAint);
        print_delegate_state("d_int_intAint (after bind)", d_int_intAint);
        TEST_ASSERT(d_int_intAint, "Delegate should be BOUND after binding member");

        // 测试绑定静态函数
        print_subsection("Binding Static Member Method");
        d_void_int.Bind(&Test::static_member_method_void_int);
        print_delegate_state("d_void_int (after bind)", d_void_int);
        TEST_ASSERT(d_void_int, "Delegate should be BOUND after binding member");
        d_void_int(10);
        TEST_ASSERT(Test::sValue == 10, "Member function invocation");
        
        // 测试绑定静态函数
        print_subsection("Binding Static Function");
        d_void_int.Bind(&static_function_void_int);
        print_delegate_state("d_void_int (after bind static)", d_void_int);
        TEST_ASSERT(d_void_int, "Delegate should be BOUND after binding static");
        d_void_int(10);
        TEST_ASSERT(sValue == 10, "Static function invocation");

        // 测试绑定全局函数
        print_subsection("Binding Global Function");
        d_int_intAint.Bind(&global_function_int_intAint);
        print_delegate_state("d_int_intAint (after bind global)", d_int_intAint);
        TEST_ASSERT(d_int_intAint, "Delegate should be BOUND after binding global");
        int result = d_int_intAint(5, 4);
        TEST_ASSERT(result == 20, "Global function invocation");

        // 测试拷贝语义测试
        print_subsection("Copy Semantics Test");
        Delegate<int, int, int> d_copy_int_intAint = d_int_intAint;
        Delegate<int, int, int> d_assign_int_intAint;
        d_assign_int_intAint = d_int_intAint;
        print_delegate_state("d_copy", d_copy_int_intAint);
        print_delegate_state("d_assign", d_assign_int_intAint);
        TEST_ASSERT(d_copy_int_intAint && d_assign_int_intAint, "Copied delegates should be BOUND");
        TEST_ASSERT(d_copy_int_intAint(2, 3) == 6 && d_assign_int_intAint(2, 3) == 6, "Copy construction/assignment invocation");

        // 移动语义测试
        print_subsection("Move Semantics Test");
        d_void_int.Bind(&t1, &Test::member_method_void_int);
        Delegate<void, int> d_move_ctor_void_int = std::move(d_void_int);
        Delegate<int, int, int> d_move_assign_void_int;
        d_move_assign_void_int = std::move(d_int_intAint);
        print_delegate_state("d_move_ctor_void_int", d_move_ctor_void_int);
        print_delegate_state("d_void_int (original)", d_void_int);
        print_delegate_state("d_move_assign_void_int", d_move_assign_void_int);
        print_delegate_state("d_int_intAint (original)", d_int_intAint);

        TEST_ASSERT(d_move_ctor_void_int, "Moved-to delegate (ctor) must be BOUND.");
        TEST_ASSERT(!d_void_int, "Moved-from delegate (ctor) must be UNBOUND.");
        TEST_ASSERT(d_move_assign_void_int, "Moved-to delegate (assign) must be BOUND.");
        TEST_ASSERT(!d_int_intAint, "Moved-from delegate (assign) must be UNBOUND.");

        d_move_ctor_void_int(5); 
        TEST_ASSERT(t1.value == 15, "Move constructor invocation");

        print_subsection("All tests complete");
        return 0;
	}

    #pragma warning( pop )
}

int main(int argc, char* argv[])
{
    Test_Delegate::Test_Delegate(argc, argv);

	sz_gui::SDLApp::InitSDLWithANGLE();

	sz_gui::SDLApp app;

	app.CreateWindow("test", 800, 600);
	
    app.SetLayout(new sz_gui::layout::AnchorLayout());

    auto frame = std::make_shared<sz_gui::widget::UIFrame>("Center",
        sz_gui::layout::AnchorPoint::Center, 
        sz_gui::layout::Margins::Percentage(5.0f, 5.0f, 5.0f, 5.0f),
        0, 0, 1.0f);
    frame->SetTexture2dUnitId(0);
    frame->SetShaderId(app.GetShaderIdByName());
    frame->SetUIFlag(sz_gui::UIFlag::Top);
    frame->SetLayout(new sz_gui::layout::AnchorLayout());
    app.RegToUI(frame);
    app.LayoutAddWidget(frame);

    auto btn1 = std::make_shared<sz_gui::widget::UIButton>("TopLeft",
        sz_gui::layout::AnchorPoint::TopLeft, 
        sz_gui::layout::Margins::Percentage(25.0f, 25.0f, 0.0f, 0.0f),
        20, 10, 1.1f);
    btn1->SetParent(frame);
    frame->AddWidget(btn1);

    auto btn2 = std::make_shared<sz_gui::widget::UIButton>("TopRight", 
        sz_gui::layout::AnchorPoint::TopRight, 
        sz_gui::layout::Margins::Percentage(0.0f, 0.0f, 25.0f, 25.0f),
        20, 10, 1.1f);
    btn2->SetParent(frame);
    frame->AddWidget(btn2);

    auto btn3 = std::make_shared<sz_gui::widget::UIButton>("Center",
        sz_gui::layout::AnchorPoint::Center, sz_gui::layout::Margins(0.0f),
        20, 10, 1.1f);
    btn3->SetParent(frame);
    frame->AddWidget(btn3);

    auto btn4 = std::make_shared<sz_gui::widget::UIButton>("BottomLeft", 
        sz_gui::layout::AnchorPoint::BottomLeft, 
        sz_gui::layout::Margins::Percentage(25.0f, 0.0f, 0.0f, 25.0f),
        20, 10, 1.1f);
    btn4->SetParent(frame);
    frame->AddWidget(btn4);

    app.Run();

	return 0;
}