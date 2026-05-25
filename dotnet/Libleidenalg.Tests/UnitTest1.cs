namespace Libleidenalg.Tests;

public class LeidenModularityTests
{
    [Fact]
    public void Partition_Throws_WhenVertexCountIsNegative()
    {
        var exception = Assert.Throws<ArgumentOutOfRangeException>(
            () => LeidenModularity.Partition(
                vertexCount: -1,
                edges: Array.Empty<(int From, int To)>(),
                directed: false,
                seed: 1,
                quality: out _));

        Assert.Equal("vertexCount", exception.ParamName);
    }

    [Fact]
    public void Partition_Throws_WhenEdgesIsNull()
    {
        var exception = Assert.Throws<ArgumentNullException>(
            () => LeidenModularity.Partition(
                vertexCount: 1,
                edges: null!,
                directed: false,
                seed: 1,
                quality: out _));

        Assert.Equal("edges", exception.ParamName);
    }
}