using System.Runtime.InteropServices;

namespace Libleidenalg;

public static class LeidenModularity
{
    public static int[] Partition(
        int vertexCount,
        IReadOnlyList<(int From, int To)> edges,
        bool directed,
        ulong seed,
        out double quality)
    {
        if (vertexCount < 0)
        {
            throw new ArgumentOutOfRangeException(nameof(vertexCount));
        }

        if (edges is null)
        {
            throw new ArgumentNullException(nameof(edges));
        }

        long[] edgePairs = new long[edges.Count * 2];
        for (var i = 0; i < edges.Count; i++)
        {
            var edge = edges[i];
            edgePairs[(i * 2)] = edge.From;
            edgePairs[(i * 2) + 1] = edge.To;
        }

        long[] membership = new long[vertexCount];

        var result = NativeMethods.libleidenalg_modularity_partition(
            vertexCount,
            edges.Count,
            edgePairs,
            directed ? 1 : 0,
            seed,
            membership,
            membership.LongLength,
            out quality);

        if (result != 0)
        {
            var messagePointer = NativeMethods.libleidenalg_last_error();
            var message = Marshal.PtrToStringAnsi(messagePointer) ?? "Unknown native error.";
            throw new LeidenException($"libleidenalg failed (code={result}): {message}");
        }

        int[] managedMembership = new int[membership.Length];
        for (var i = 0; i < membership.Length; i++)
        {
            managedMembership[i] = checked((int)membership[i]);
        }

        return managedMembership;
    }
}
