Regular (multi-passable) generator view.

```cpp
auto i0_10 =  view::generate_while(
    [i = int()](auto rewind) mutable -> std::optional<int>{
        if constexpr (rewind) i = -1;
        if (i == 10) return {};
        return {++i};
    });
    
for(int i : i0_100) std::cout << i << " ";
std::cout << std::endl;
for(int i : i0_100) std::cout << i << " ";
```

Output:
```
0 1 2 3 4 5 6 7 8 9 10
0 1 2 3 4 5 6 7 8 9 10
```
