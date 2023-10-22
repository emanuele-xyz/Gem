#include <BtL\pch.h>
#include <BtL\BtL.h>
#include <Gem\Gem.h>

int main()
{
	std::cout << "Hello from Battling Through the Lands!" << std::endl;
	Gem::Hello();

	try
	{
		GemCrash("crash");
	}
	catch (const Gem::Crash& e)
	{
		Gem::ShowErrorPopup(e.what());
		std::cout << e.what() << std::endl;
	}

	return 0;
}
