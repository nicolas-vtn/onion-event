#include <iostream>

#include <onion/Event.hpp>

class ExampleEventArgs
{
  public:
	int value;
	ExampleEventArgs(int v) : value(v) {}
};

class ExampleClass
{
  public:
	int data = 42;

  public:
	void sayEventValue(const ExampleEventArgs& args) const
	{
		std::cout << "Event value: " << args.value << ", data: " << data << std::endl;
	}
};

int main()
{
	std::cout << "---------- Demo Event class ----------" << std::endl;

	// Create an ExampleClass instance
	ExampleClass example;

	// Create an Event for ExampleEventArgs
	onion::Event<ExampleEventArgs> event;

	{
		// Creates a lambda handler that captures the example instance by reference
		onion::EventHandle eventHandle_1 =
			event.Subscribe([&example](const ExampleEventArgs& args) { example.sayEventValue(args); });

		// Create another handler that captures the example instance by reference
		onion::EventHandle eventHandle_2 =
			event.Subscribe([&example](const ExampleEventArgs& args) { example.sayEventValue(args); });

		// Trigger the event with some arguments
		std::cout << "\nTriggering event with value 100..." << std::endl;
		event.Trigger(ExampleEventArgs(100));

		// Unsubscribe the first handle
		event.Unsubscribe(eventHandle_1);

		// Trigger the event again to show that the first handle has been unsubscribed
		std::cout << "\nTriggering event with value 150 after unsubscribing first handle..." << std::endl;
		event.Trigger(ExampleEventArgs(150));

	} // The eventHandle goes out of scope here, automatically unsubscribing the handle

	// Trigger the event again to show that the handle has been unsubscribed
	std::cout << "\nTriggering event with value 200 after handle has been unsubscribed ..." << std::endl;
	event.Trigger(ExampleEventArgs(200));

	return 0;
}
