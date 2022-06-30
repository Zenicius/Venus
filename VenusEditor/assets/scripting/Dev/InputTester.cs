using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Venus;

namespace Dev
{
    public class InputTester : Entity
    {
        void Create()
        {
            Log.Warn("Created Input Tester!!  / Entity UUID : " + ID);
        }

        void Update(float Timestep)
        {
            if (Input.IsMousePressed(MouseCode.ButtonLeft))
                Log.Trace("Botao Esquerdo");

            if (Input.IsMousePressed(MouseCode.ButtonRight))
                Log.Trace("Botao Direito");
        }
    }
}
