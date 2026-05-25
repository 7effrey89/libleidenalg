# Libleidenalg.Wrapper

Managed .NET wrapper for the native `libleidenalg` library.

## Native dependency

This package expects the native `libleidenalg` shared library to be available at runtime.

## Example

```csharp
using Libleidenalg;

var edges = new List<(int From, int To)>
{
    (0, 1),
    (1, 2),
    (2, 0),
    (3, 4)
};

double quality;
int[] membership = LeidenModularity.Partition(
    vertexCount: 5,
    edges: edges,
    directed: false,
    seed: 42,
    quality: out quality);
```
