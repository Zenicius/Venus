using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Venus;

namespace Dev
{
    public class EntityTest : Entity
    {
        private Entity m_Camera;
        private Entity m_RedLight;
        private float m_Time = 0;

        void Start()
        {
            Log.Trace("Entity Initialized");
            Log.Trace("ID-> " + ID);

            Entity searched = FindEntityByName("Geometry");
            searched.GetComponent<TagComponent>().Name = "Me Acharam!";

            Entity newEntity = Create();

            newEntity.Parent = this;

            if(HasParent())
            {
                Entity parent = Parent;
                Log.Trace("Parent Name-> " + parent.Name);
            }

            Entity[] children = Children;
            Log.Trace("Total Children-> " + children.Length);

            foreach (Entity child in children)
            {
                child.GetComponent<TagComponent>().Name = "Sou Filho!";
            }

            if(HasComponent<TagComponent>())
            {
                string name = GetComponent<TagComponent>().Name;
                Log.Trace("Name-> " + name);

                string newName = "João";
                Log.Trace("Setting new name to-> " + newName);
                GetComponent<TagComponent>().Name = newName;
            }

            if(HasComponent<TransformComponent>())
            {
                Transform transform = GetComponent<TransformComponent>().Transform;
                Log.Trace("Position-> " + transform.Position.ToString());
                Log.Trace("Rotation-> " + transform.Rotation.ToString());
                Log.Trace("Scale-> " + transform.Scale.ToString());
            }

            PointLightComponent pointLight = AddComponent<PointLightComponent>();
            pointLight.Color = new Vector3(0.0f, 1.0f, 0.0f);
            pointLight.Intensity = 5.0f;

            m_Camera = FindEntityByName("Camera");
            m_RedLight = FindEntityByName("Red Light");

            Log.Error("Test Error!");
            Log.Critical("Test Critical!");
            Log.Warn("Test Warn");

            //Destroy();
        }

        void Update(float Timestep)
        {
         
            if (m_Camera != null)
            {
                Vector3 position = m_Camera.Position;
                position.X += 1.0f * Timestep;
                m_Camera.Position = position;
            }

            if (m_RedLight != null)
            {
                if (m_Time >= 1.0f)
                {
                    float intensity = m_RedLight.GetComponent<PointLightComponent>().Intensity;

                    if (intensity > 0.0f)
                    {
                        m_RedLight.GetComponent<PointLightComponent>().Intensity = 0.0f;
                    }
                    else
                        m_RedLight.GetComponent<PointLightComponent>().Intensity = 5.0f;

                    m_Time = 0.0f;
                }
            }

            m_Time += 1.0f * Timestep;
        }
    }
}
