using System.Runtime.InteropServices;

namespace Libleidenalg;

internal static class NativeMethods
{
    private const string LibraryName = "libleidenalg";

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int libleidenalg_modularity_partition(
        long vertex_count,
        long edge_count,
        long[] edges,
        int directed,
        ulong seed,
        long[] membership_out,
        long membership_len,
        out double quality_out);

    [DllImport(LibraryName, CallingConvention = CallingConvention.Cdecl)]
    internal static extern IntPtr libleidenalg_last_error();
}
