using System;
using System.Runtime.InteropServices;

namespace Venus
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Vector3
    {
        public float X, Y, Z;

        public Vector3(float value)
        {
            X = value;
            Y = value;
            Z = value;
        }

        public Vector3(float x, float y, float z)
        {
            X = x;
            Y = y;
            Z = z;
        }

        public override string ToString() 
        {
            return "X: " + X + " Y: " + Y + " Z: " + Z;
        }
    }
}
