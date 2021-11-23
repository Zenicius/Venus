#include <Venus.h>

class SandBox : public Venus::Application
{
	public:
		SandBox()
		{

		}

		~SandBox()
		{

		}
};

Venus::Application* Venus::CreateApplication()
{
	return new SandBox();
}