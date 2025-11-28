#include "app.h"

#include "xen_vector.h"
#include "arena.h"

#include <iostream>
#include <algorithm>


struct NonTrivial {
    char* ptr;
    NonTrivial(Arena a) : arena_(a) 
    {
        std::cout << "NT constr" << std::endl;
        ptr = a.Construct<char>(); 
    }

    ~NonTrivial()
    {
        std::cout << "NT destruct" << std::endl;
    }
private:
    Arena arena_;
};
struct Trivial {
    char c;
    Trivial() { c = 'c'; }
    ~Trivial() = default;
};

struct OuterNonTrivial {
    NonTrivial* nt;
    OuterNonTrivial(Arena a) { std::cout << "ONT construct" << std::endl; nt = a.Construct<NonTrivial>(a);  };
    ~OuterNonTrivial()
    {
        std::cout << "ONT destruct" << std::endl; 
    };

};
bool App::StartUp()
{
    size_t gb = 1024 * 1024 * 1024;
    Mem::InitInstance({ gb * 2, gb * 1});
    //Allocator allocator = Mem::GetInstance()->CreateAllocator();
    //Trivial* trivial =  Mem::GetInstance()->Allocate<Trivial>(allocator);
    //NonTrivial* non =  Mem::GetInstance()->Allocate<NonTrivial>(allocator);
    //Mem::GetInstance()->GetDiagnostics(); // Shows allocated blocks


    //Mem::GetInstance()->Reset(allocator);
    //Mem::GetInstance()->GetDiagnostics();
    //Mem::DestroyInstance();
    return true;
}


bool App::TestVector()
{
    //ArenaID allocator = Mem::GetInstance()->main_arena_pool->CreateArena();

    //Vector<Trivial> v1(allocator);                    // Default: empty vector
    //v1.Init(5);                        // Size: 5 default-constructed ints (0)
    //v1.Print();
    //Vector<int> v3(3, 42);           // Size and value: 3 copies of 42
    //Vector<int> v4 = { 1, 2, 3 };      // Initializer list: [1, 2, 3]
    //Vector<int> v5(v3);              // Copy: copy of v3
    //Vector<int> v6(std::move(v3));   // Move: v3 is now empty
    //Vector<int> v7(v4.begin(), v4.end()); // Range: copy of v4
    Arena a{ (ArenaID)Arena::Node, (ThreadID)Threads::Main };
    //int* i = a.Construct<int>();
    //*i = 4;

    //Trivial* t = a.ConstructAt(a.Alloc<Trivial>());

    //NonTrivial* nt = a.ConstructAt(a.Alloc<NonTrivial>(1));
     
    OuterNonTrivial* nt = a.ConstructAt(a.Alloc<OuterNonTrivial>(1), a);


    Mem::GetInstance()->PrintDiagnostics();
    a.Reset();
    Mem::GetInstance()->PrintDiagnostics();
    //std::vector<int> vec(5);
  /*  for (size_t i = 0; i < vec.Size(); ++i)
        vec[i] = static_cast<int>(i);

    std::for_each(vec.begin(), vec.end(), [](int val) {
        std::cout << val << " ";
    });
    std::cout << "\n";

    std::reverse(vec.begin(), vec.end());
    for (auto val : vec)
        std::cout << val << " ";
    std::cout << "\n";*/

    //Mem::GetInstance()->Reset(allocator);
    return true;
}

bool App::Shutdown()
{
   // Mem::GetInstance()->GetDiagnostics();
    Mem::DestroyInstance();
    return false;
}
