#include <Venus.h>

class Sandbox : public Venus::Application
{
	public:
		Sandbox()
		{

		}

		~Sandbox()
		{

		}
};

Venus::Application* Venus::CreateApplication()
{
	return new Sandbox();
}