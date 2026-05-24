# libleidenalg for .NET / C#

This repository provides a native implementation of the Leiden algorithm with a
.NET wrapper so C# applications can adopt Leiden community detection directly.
The recommended adoption path in this repository is:

1. Use the managed wrapper package (`Libleidenalg.Wrapper`) in your C# code.
2. Provide the native `libleidenalg` runtime binary (for example via a native NuGet package).

The wrapper lives in `dotnet/Libleidenalg` and calls the C ABI exposed by
`include/c_api.h`.

# C# quick start

Add the wrapper to your project:

```bash
dotnet add package Libleidenalg.Wrapper
```

Then call the modularity partition API:

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

> The managed package expects the native `libleidenalg` shared library to be
> available at runtime.

# Build and pack the C# wrapper

Use the .NET solution in `dotnet/Libleidenalg.DotNet.slnx`:

```bash
dotnet restore dotnet/Libleidenalg.DotNet.slnx
dotnet build dotnet/Libleidenalg.DotNet.slnx --configuration Release
dotnet test dotnet/Libleidenalg.DotNet.slnx --configuration Release
dotnet pack dotnet/Libleidenalg/Libleidenalg.csproj --configuration Release
```

# Build native NuGet package for C# interop

You can generate a native NuGet package (`.nupkg`) from the CMake install
output. This package contains native `libleidenalg` artifacts consumable from
C# interop layers.

1. Configure with a local install prefix (Windows example):

   ```powershell
   cmake --preset msbuild-vcpkg -DCMAKE_INSTALL_PREFIX=$PWD\builds\msbuild-vcpkg\install
   ```

2. Build and install Release artifacts:

   ```powershell
   cmake --build --preset msbuild-vcpkg --config Release --target install
   ```

3. Create the NuGet package:

   ```powershell
   cpack --config builds\msbuild-vcpkg\CPackConfig.cmake -G NuGet -C Release
   ```

4. (Optional) Publish to NuGet.org:

   ```powershell
   dotnet nuget push builds\msbuild-vcpkg\libleidenalg.native.<version>.nupkg --source https://api.nuget.org/v3/index.json --api-key <NUGET_API_KEY>
   ```

# Native C++ library (core)

The core implementation is in `C++` and relies on `igraph`. It scales to large
graphs and exposes the `Optimiser` API for Leiden optimisation across:

1. Modularity[^3]
2. Reichardt and Bornholdt models[^4]
3. Constant Potts Model (CPM)[^5]
4. Significance[^6]
5. Surprise[^7]

It also supports multiplex optimisation[^8][^9], partially fixed assignments[^10],
and bipartite use cases.

Most users focused on C# should prefer the wrapper workflow above. Other language
interfaces include Python (https://github.com/vtraag/leidenalg) and Elixir
(https://github.com/georgeguimaraes/leidenfold).

# Native C++ build and install

The native build system uses CMake:

```bash
mkdir build && cd build
cmake ..
cmake --build .
cmake --build . --target install
```

You can customize install location with
[`CMAKE_INSTALL_PREFIX`](https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html)
and static/shared output with
[`BUILD_SHARED_LIBS`](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html).

This library depends on `igraph`: https://igraph.org/c/doc/igraph-Installation.html

If `igraph` is in a non-standard location, set
[`CMAKE_PREFIX_PATH=<dir>`](https://cmake.org/cmake/help/latest/variable/CMAKE_PREFIX_PATH.html)
when configuring `libleidenalg`.

# Usage

The `Optimiser` class is responsible for optimising a `MutableVertexPartition` (possibly multiple in the case of a multiplex approach). The `MutableVertexPartition` is just a base class, and should be implemented to provide explicit quality function:
- `CPMVertexPartition`
- `ModularityVertexPartition`
- `RBConfigurationVertexPartition`
- `RBERVertexPartition`
- `SignificanceVertexPartition`
- `SurpriseVertexPartition`

Some of these classes depend on intermediate derived classes. The implementation of a quality function in a derived class, essentially comes to down implementing the `diff_move` and `quality`, see also [`CONTRIBUTING.md`](CONTRIBUTING.md) in this repository.

An `igraph_t` object from `igraph` is used to construct a separate `Graph` object, which can be used to construct a `MutableVertexPartition`. For example, to find a partition using CPM, you could do the following

```C
    igraph_t g;
    igraph_famous(&g, "Zachary");

    Graph graph(&g);

    CPMVertexPartition part(&graph,
                            0.05 /* resolution */ );

    Optimiser o;

    o.optimise_partition(&part);
```

In the `example` directory, we added a complete example, including the CMake build files (please note that this directory is not included in the release source package, and only available on the GitHub repository). In order to compile it, first make sure you have properly installed `igraph` and `libleidenalg`. In the `example` directory, you can then follow the standard CMake routine to build it
```bash
mkdir build && cd build
cmake ..
cmake --build .
```
and run `./example`.

Licence
-------

Copyright (C) 2020 V.A. Traag, (C) 2022 Andrew Robbins

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see http://www.gnu.org/licenses/.

# References

Please cite the references appropriately in case they are used.

[^1]:  Traag, V.A., Waltman. L., Van Eck, N.-J. (2018). From Louvain to
       Leiden: guaranteeing well-connected communities. Scientific reports, 9(1), 5233.
       [10.1038/s41598-019-41695-z](http://dx.doi.org/10.1038/s41598-019-41695-z)

[^2]:  Blondel, V. D., Guillaume, J.-L., Lambiotte, R., & Lefebvre, E. (2008).
       Fast unfolding of communities in large networks. Journal of Statistical
       Mechanics: Theory and Experiment, 10008(10), 6.
       [10.1088/1742-5468/2008/10/P10008](http://doi.org/10.1088/1742-5468/2008/10/P10008)

[^3]:  Newman, M. E. J., & Girvan, M. (2004). Finding and evaluating community
       structure in networks. Physical Review E, 69(2), 026113.
       [10.1103/PhysRevE.69.026113](http://doi.org/10.1103/PhysRevE.69.026113)

[^4]:  Reichardt, J., & Bornholdt, S. (2006). Statistical mechanics of
       community detection. Physical Review E, 74(1), 016110.
       [10.1103/PhysRevE.74.016110](http://doi.org/10.1103/PhysRevE.74.016110)

[^5]:  Traag, V. A., Van Dooren, P., & Nesterov, Y. (2011). Narrow scope for
       resolution-limit-free community detection. Physical Review E, 84(1),
       1. [10.1103/PhysRevE.84.016114](http://doi.org/10.1103/PhysRevE.84.16114)

[^6]:  Traag, V. A., Krings, G., & Van Dooren, P. (2013). Significant scales in
       community structure. Scientific Reports, 3, 2930. [10.1038/srep02930](http://doi.org/10.1038/srep02930)

[^7]:  Traag, V. A., Aldecoa, R., & Delvenne, J.-C. (2015). Detecting
       communities using asymptotical surprise. Physical Review E, 92(2),
       1. [10.1103/PhysRevE.92.022816](http://doi.org/10.1103/PhysRevE.92.022816)

[^8]:  Traag, V. A., & Bruggeman, J. (2009). Community detection in networks
       with positive and negative links. Physical Review E, 80(3), 036115.
       [10.1103/PhysRevE.80.036115](http://doi.org/10.1103/PhysRevE.80.036115)

[^9]:  Mucha, P. J., Richardson, T., Macon, K., Porter, M. A., & Onnela, J.-P.
       (2010). Community structure in time-dependent, multiscale, and multiplex
       networks. Science, 328(5980), 876–8. [10.1126/science.1184819](http://doi.org/10.1126/science.1184819)

[^10]:  Zanini, F., Berghuis, B. A., Jones, R. C., Robilant, B. N. di,
        Nong, R. Y., Norton, J., Clarke, Michael F., Quake, S. R. (2019).
        northstar: leveraging cell atlases to identify healthy and neoplastic
        cells in transcriptomes from human tumors. BioRxiv, 820928.
        [10.1101/820928](https://doi.org/10.1101/820928)
