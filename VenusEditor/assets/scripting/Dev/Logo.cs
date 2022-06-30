using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Venus;

namespace Dev
{
    public class Logo : Entity
    {
        private Entity m_Camera;
        private Entity m_Camera2;
        private Entity m_RedLight;
        private float m_Time = 0;

        void Start()
        {
            m_Camera = FindEntityByName("Camera");
            m_Camera2 = FindEntityByName("Camera2");
            m_RedLight = FindEntityByName("RedLight");

            
            Log.Trace("Primary Camera->  " + m_Camera.GetComponent<CameraComponent>().Primary);
            Log.Trace("Primary Camera2->  " + m_Camera2.GetComponent<CameraComponent>().Primary);
            Log.Warn("Test");
        }

        void Update(float Timestep)
        {
            // Camera Movement
            Vector3 position = m_Camera.Position;
            position.Z -= 1.0f * Timestep;
            m_Camera.Position = position;

            // Turn on/off light
            if (m_Time >= 1.0f)
            {
                float intensity = m_RedLight.GetComponent<PointLightComponent>().Intensity;

                if (intensity > 0.0f)
                {
                    m_RedLight.GetComponent<PointLightComponent>().Intensity = 0.0f;
                }
                else
                    m_RedLight.GetComponent<PointLightComponent>().Intensity = 8.5f;

                m_Time = 0.0f;
            }

            // Change Camera / And light color
            if(position.Z < 0.0f)
            {
                m_Camera.GetComponent<CameraComponent>().Primary = false;
                m_Camera2.GetComponent<CameraComponent>().Primary = true;

                Vector3 NewColor = new Vector3(0.0f, 0.0f, 1.0f);
                m_RedLight.GetComponent<PointLightComponent>().Color = NewColor;
            }

            m_Time += Timestep * 1.0f;
        }
    }
}
