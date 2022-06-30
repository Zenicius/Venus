using System;
using System.Runtime.CompilerServices;

namespace Venus
{
    internal enum Level
    {
        Trace = 0,
        Info = 1, 
        Warn = 2, 
        Error = 3,
        Critical = 4
    }

    public static class Log
    {
        public static void Trace(object message) => Log_VenusEngine(Level.Trace, message.ToString());
        public static void Info(object message) => Log_VenusEngine(Level.Info, message.ToString());
        public static void Warn(object message) => Log_VenusEngine(Level.Warn, message.ToString());
        public static void Error(object message) => Log_VenusEngine(Level.Error, message.ToString());
        public static void Critical(object message) => Log_VenusEngine(Level.Critical, message.ToString());

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Log_VenusEngine(Level level, string message);
    }
}
