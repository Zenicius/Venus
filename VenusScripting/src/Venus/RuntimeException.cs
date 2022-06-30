using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Venus
{
    public class RuntimeException
    {
        public static void OnException(object e)
        {
            string msg = "RuntimeException: ";
            if(e != null)
            {
                if(e is NullReferenceException)
                {
                    var exception = e as NullReferenceException;
                    msg += exception.Message;
                    msg += " ";
                    msg += exception.Source;
                    msg += " ";
                    msg += exception.StackTrace;

                    Log.Error(msg);
                }
                else if(e is Exception)
                {
                    var exception = e as Exception;
                    msg += exception.Message;
                    msg += " ";
                    msg += exception.Source;
                    msg += " ";
                    msg += exception.StackTrace;

                    Log.Error(msg);
                }
            }
            else
            {
                msg += "Exception is Null";
            }
        }
    }
}
