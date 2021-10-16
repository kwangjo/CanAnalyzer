Some C++11 features required by FakeIt are not supported by MSVS C++ 2013. To overcome this, FakeIt also contains standard C++ alternatives that compile on both MSVS C++ 2013 and GCC 4.8.
All of the code samples in this Quickstart can be compiled by both MSVS C++ 2013 and GCC 4.8.

* [Stubbing](#stubbing)
* [Invocation Matching](#invocation-matching)
* [Verification](#verification)
* [Spying](#spying)
* [Reset Mock to Initial State](#reset-mock-to-initial-state)
* [Inheritance & Dynamic Casting](#inheritance--dynamic-casting)

Assuming we have the following interface:
```C++
struct SomeInterface {
   virtual int foo(int) = 0;
   virtual int bar(int,int) = 0;
};
```
## Stubbing
```C++
// Stub a method to return a value once
When(mock[&SomeInterface::foo]).Return(1);

// Stub multiple return values (The next two lines do exactly the same)
When(mock[&SomeInterface::foo]).Return(1,2,3);
When(mock[&SomeInterface::foo]).Return(1).Return(2).Return(3);

// Return the same value many times (56 in this example)
When(mock[&SomeInterface::foo]).Return(Times<56>::of(1));

// Return many values many times (First 100 calls will return 1, next 200 calls will return 2)
When(mock[&SomeInterface::foo]).Return(Times<100>::of(1), Times<200>::of(2));

// Always return a value (The next two lines do exactly the same)
When(mock[&SomeInterface::foo]).AlwaysReturn(1);
mock[&SomeInterface::foo] = 1;
```
**What if I want to be more specific?**
```C++
// Stub foo(1) to return the value '100' once (The next two lines do the same)
When(mock[&SomeInterface::foo].Using(1)).Return(100);
When(mock[&SomeInterface::foo](1)).Return(100);

// Stub 'foo(1)' to always return '100'. For all other calls always return 0.
When(mock[&SomeInterface::foo]).AlwaysReturn(0); // Any invocation of foo will return 0
When(mock[&SomeInterface::foo].Using(1)).AlwaysReturn(100); // override only for 'foo(1)'

// The next two lines do exactly the same
When(mock[&SomeInterface::foo].Using(1)).AlwaysReturn(0);
mock[&SomeInterface::foo].Using(1) = 0;
```
**Can I stub a method to throw an exception?**
```C++
// Throw once
When(mock[&SomeInterface::foo]).Throw(exception());
// Throw several times
When(mock[&SomeInterface::foo]).Throw(exception(),exception());
// Throw many times
When(mock[&SomeInterface::foo]).Throw(Times<23>::of(exception()));
// Always throw
When(mock[&SomeInterface::foo]).AlwaysThrow(exception());
```
**Nice, but sometimes I need something more freestyle**
```C++
// Do whatever you want using lambda expressions
When(mock[&SomeInterface::foo]).Do([](int a)->int{ ... });
When(mock[&SomeInterface::foo]).AlwaysDo([](int a)->int{ ... });
```
## Invocation Matching
```C++
// Stub foo to return 1 only when argument 'a' is even.
auto agrument_a_is_even = [](int a){return a%2==0;};
When(mock[&SomeInterface::foo].Matching(agrument_a_is_even)).Return(1);

// Throw exception only when argument 'a' is negative.
auto agrument_a_is_negative = [](int a){return a < 0;};
When(mock[&SomeInterface::foo].Matching(agrument_a_is_negative)).Throw(exception());

// Stub bar to throw exception only when argument 'a' is bigger than argument 'b'.
auto a_is_bigger_than_b = [](int a, int b){return a > b;};
When(mock[&SomeInterface::bar].Matching(a_is_bigger_than_b)).Throw(exception());
```

## Verification
```C++
Mock<SomeInterface> mock;
When(mock[&SomeInterface::foo]).AlwaysReturn(1);

SomeInterface & i = mock.get();

// Production code:
i.foo(1);
i.foo(2);
i.foo(3);

// Verify foo was invoked at least once. (The four lines do exactly the same)
Verify(mock[&SomeInterface::foo]);
Verify(mock[&SomeInterface::foo]).AtLeastOnce();
Verify(mock[&SomeInterface::foo]).AtLeast(1);
Verify(mock[&SomeInterface::foo]).AtLeast(Times<1>());

// Verify foo was invoked at exactly 3 times. (The next two lines do exactly the same)
Verify(mock[&SomeInterface::foo]).Exactly(3);
Verify(mock[&SomeInterface::foo]).Exactly(Times<3>());

// Verify foo(1) was invoked exactly once
Verify(mock[&SomeInterface::foo].Using(1)).Once();
Verify(mock[&SomeInterface::foo].Using(1)).Exactly(Once);
```
**Wow, can I verify the order of invocations too?**
```C++
// Verify foo(1) was invoked before foo(3)
Verify(mock[&SomeInterface::foo].Using(1), mock[&SomeInterface::foo].Using(3));
```
**What about an exact sequence? Can I verify an exact sequence of invocations?**

Sure, you represent a sequence in the following way:

Two consecutive invocations of foo:
```C++
mock[&SomeInterface::foo] * 2
```
An invocation of foo followed by an invocation of bar:
```C++
mock[&SomeInterface::foo] + mock[&SomeInterface::bar]
```
Two consecutive invocations of foo + bar, i.e. foo + bar + foo + bar
```C++
(mock[&SomeInterface::foo] + mock[&SomeInterface::bar]) * 2
```
This way you can represent any sequence of invocations.

To verify that a specific sequence exists in the actual invocation sequence simply write:
```C++
// verify the actual invocation sequence contains two consecutive invocations of foo at least once.
Verify(mock[&SomeInterface::foo] * 2); 

// verify the actual invocation sequence contains two consecutive invocations of foo exactly once.
Verify(mock[&SomeInterface::foo] * 2).Exactly(Once);

// verify the actual invocation sequence contains an invocation of foo(1) followed by bar(1,2) exactly twice.
Verify(mock[&SomeInterface::foo].Using(1) + mock[&SomeInterface::bar].Using(1,2)).Exactly(Times<2>());
```
**Can a sequence involve more than one mock instance?**

Sure, a sequence can involve multiple mock instances.
```C++
Mock<SomeInterface> mock1;
Mock<SomeInterface> mock2;

When(mock1[&SomeInterface::foo]).AlwaysReturn(0);
When(mock2[&SomeInterface::foo]).AlwaysReturn(0);

SomeInterface & i1 = mock1.get();
SomeInterface & i2 = mock2.get();

// Production code:
i1.foo(1);
i2.foo(1);
i1.foo(2);
i2.foo(2);
i1.foo(3);
i2.foo(3);

// Verify exactly 3 occurrences of the sequence {mock1.foo(any int) + mock2.foo(any int)}.
Verify(mock1[&SomeInterface::foo] + mock2[&SomeInterface::foo]).Exactly(Times<3>());

```
  
### Verify No Other Invocations
```C++
Mock<SomeInterface> mock;
When(mock[&SomeInterface::foo]).AlwaysReturn(0);
When(mock[&SomeInterface::bar]).AlwaysReturn(0);
SomeInterface& i  = mock.get();

// call foo twice and bar once.
i.foo(1);
i.foo(2);
i.bar("some string");

// verify foo(1) was called.
Verify(mock[&SomeInterface::foo].Using(1));

// Verify no other invocations of any method of mock. 
// Will fail since foo(2) & bar("some string") are not verified yet.
VerifyNoOtherInvocations(mock);

// Verify no other invocations of method foo only.
// Will fail since foo(2) is not verified yet.
VerifyNoOtherInvocations(mock[&SomeInterface::foo]);
 
Verify(mock[&SomeInterface::foo].Using(2));

// Verify no other invocations of any method of mock. 
// Will fail since bar("some string") is not verified yet.
VerifyNoOtherInvocations(mock);

// Verify no other invocations of method foo only.
// Will pass since both foo(1) & foo(2) are now verified.
VerifyNoOtherInvocations(mock[&SomeInterface::foo]);

Verify(mock[&SomeInterface::bar]); // verify bar was invoked (with any arguments)

// Verify no other invocations of any method of mock. 
// Will pass since foo(1) & foo(2) & bar("some string") are now verified.
VerifyNoOtherInvocations(mock);.
```
## Reset Mock to Initial State
In most cases you will need to reset the mock objects to the initial state before/after each test method. To do that simply add the following line for each mock object to the setup/teardown code of your tests.
```C++
mock.Reset();
```
## Spying
In some cases it is very useful to spy an existing object. FakeIt is the ONLY C++ open source mocking framework that supports spying.
```C++
class SomeClass {
public:
   virtual int func1(int arg) {
      return arg;
   }
   virtual int func2(int arg) {
      return arg;
   }
};

SomeClass obj;
Mock<SomeClass> spy(obj);

When(spy[&SomeClass::func1]).AlwaysReturn(10); // Override to return 10

SomeClass& i = spy.get();
cout << i.func1(1); // will print 10. 
cout << i.func2(1); // func2 is not stubbed.
```
In general, all stubbing & verifying features work on spy objects the same way they work on mock objects.
## Inheritance & Dynamic Casting

```C++
struct A {
  virtual int foo() = 0;
};

struct B : public A {
  virtual int foo() override = 0;
};

struct C : public B
{
   virtual int foo() override = 0;
};
```
**upcast support**
```C++
Mock<C> cMock;		
When(cMock[&C::foo]).AlwaysReturn(0);

C& c = cMock.get();
B& b = c;
A& a = b;

cout << c.foo(); // prints 0
cout << b.foo(); // prints 0
cout << a.foo(); // prints 0
```
**dynamic_cast support**
```C++
Mock<C,B,A> cMock;
When(cMock[&C::foo]).AlwaysReturn(0);

A& a = cMock.get(); // get instance and upcast to A&

B& b = dynamic_cast<B&>(a); // downcast to B&
cout << b.foo(); // prints 0

C& c = dynamic_cast<C&>(a); // downcast to C&
cout << c.foo(); // prints 0
```