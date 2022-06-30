using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Venus;

namespace Dev
{
    public class Logger
    {
        public float time = 0.0f;

        void Create()
        {
            Log.Warn("Created Logger!!");
        }

        void Update(float Timestep)
        {
            time += Timestep;

            if(time >= 5.0f)
            {
                time = 0.0f;

                Log.Warn("Logger logging!");
            }
        }
    }
}
