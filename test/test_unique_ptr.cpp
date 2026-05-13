#include "../includes/_unique_ptr.hpp"
#include <iostream>
#include <cstdio>

using namespace std;

struct Base
{
    virtual ~Base() = default;
    virtual const char *who() const { return "Base"; }
};

struct Derived : Base
{
    const char *who() const override { return "Derived"; }
};

struct FileDeleter
{
    void operator()(FILE *fp) const
    {
        if (fp)
        {
            cout << "File closed successfully\n";
            fclose(fp);
        }
    }
};

int main()
{
    cout << "=== Test 1: Basic int pointer ===\n";
    {
        Unique_ptr<int> ptr(new int(42));
        cout << "Value: " << *ptr << endl;
        cout << "Get: " << *ptr.get() << endl;
    }
    cout << "Int pointer destroyed\n\n";

    cout << "=== Test 2: String pointer ===\n";
    {
        Unique_ptr<string> ptr(new string("Hello, World!"));
        cout << "String: " << *ptr << endl;
        cout << "Length: " << ptr->length() << endl;
    }
    cout << "String pointer destroyed\n\n";

    cout << "=== Test 3: Move semantics (same type) ===\n";
    {
        Unique_ptr<int> ptr1(new int(100));
        cout << "Before move: " << *ptr1 << endl;

        Unique_ptr<int> ptr2(std::move(ptr1));
        cout << "After move ptr2: " << *ptr2 << endl;
        cout << "ptr1 is null: " << (ptr1 == nullptr) << endl;
    }
    cout << "Move test passed\n\n";

    cout << "=== Test 4: Polymorphism (Derived to Base) ===\n";
    {
        Unique_ptr<Derived> derived(new Derived());
        cout << "Derived who: " << derived->who() << endl;

        Unique_ptr<Base> base(std::move(derived));
        cout << "Base who: " << base->who() << endl;
        cout << "derived is null: " << (derived == nullptr) << endl;
    }
    cout << "Polymorphism test passed\n\n";

    cout << "=== Test 5: Array support ===\n";
    {
        Unique_ptr<int[]> arr(new int[5]);
        for (int i = 0; i < 5; ++i)
        {
            arr[i] = (i + 1) * 10;
        }
        cout << "Array elements: ";
        for (int i = 0; i < 5; ++i)
        {
            cout << arr[i] << " ";
        }
        cout << endl;
    }
    cout << "Array test passed\n\n";

    cout << "=== Test 6: Custom FileDeleter ===\n";
    {
        FILE *fp = fopen("test.txt", "w");
        if (fp)
        {
            Unique_ptr<FILE, FileDeleter> file(fp);
            fprintf(file.get(), "Test content\n");
            cout << "File written successfully\n";
        }
    }
    cout << "FileDeleter test passed\n\n";

    cout << "=== Test 7: Release and Reset ===\n";
    {
        Unique_ptr<int> ptr(new int(999));
        cout << "Before release: " << *ptr << endl;

        int *raw = ptr.release();
        cout << "After release, ptr is null: " << (ptr == nullptr) << endl;
        cout << "Raw pointer value: " << *raw << endl;
        delete raw;
    }
    cout << "Release test passed\n\n";

    cout << "=== Test 8: Reset ===\n";
    {
        Unique_ptr<int> ptr(new int(111));
        cout << "Before reset: " << *ptr << endl;

        ptr.reset(new int(222));
        cout << "After reset: " << *ptr << endl;

        ptr.reset();
        cout << "After reset to nullptr: " << (ptr == nullptr) << endl;
    }
    cout << "Reset test passed\n\n";

    cout << "=== Test 9: Swap ===\n";
    {
        Unique_ptr<int> ptr1(new int(10));
        Unique_ptr<int> ptr2(new int(20));
        cout << "Before swap - ptr1: " << *ptr1 << ", ptr2: " << *ptr2 << endl;

        ptr1.swap(ptr2);
        cout << "After swap - ptr1: " << *ptr1 << ", ptr2: " << *ptr2 << endl;
    }
    cout << "Swap test passed\n\n";

    cout << "=== Test 10: Bool conversion ===\n";
    {
        Unique_ptr<int> ptr1(new int(42));
        Unique_ptr<int> ptr2;

        if (ptr1)
            cout << "ptr1 is valid\n";
        if (!ptr2)
            cout << "ptr2 is empty\n";
    }
    cout << "Bool conversion test passed\n\n";

    cout << "=== All tests passed! ===\n";
    return 0;
}