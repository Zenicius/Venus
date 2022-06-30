using System;
using System.Runtime.CompilerServices;

namespace Venus
{
    public class Entity
    {
        public ulong ID { get; private set; }

        protected Entity() { ID = 0; }

        internal Entity(ulong iD)
        {
            ID = iD;
        }

        public string Name => GetComponent<TagComponent>().Name;

        //-- Management----------------------------------------------------------------------------------
        public Entity Create()
        {
            return new Entity(CreateEntity_VenusEngine());
        }

        public Entity FindEntityByName(string name)
        {
            ulong entityID = FindEntityByName_VenusEngine(name);

            if (entityID == 0)
                return null;

            return new Entity(entityID);
        }

        public Entity FindEntityByID(ulong entityID)
        {
            if (ExistEntity_VenusEngine(entityID))
                return new Entity(entityID);
            else
                return null;
        }

        public void Destroy()
        {
            DestroyEntity_VenusEngine(ID);
        }

        public void Destroy(Entity entity)
        {
            DestroyEntity_VenusEngine(entity.ID);
        }
        //-----------------------------------------------------------------------------------------------



        //-- Relationship--------------------------------------------------------------------------------
        public Entity Parent
        {
            get => new Entity(GetParent_VenusEngine(ID));

            set
            {
                if (value == this)
                    return;

                SetParent_VenusEngine(ID, value.ID);
            }
        }

        public bool HasParent()
        {
            return HasParent_VenusEngine(ID);
        }

        public Entity[] Children
        {
            get => GetChildren_VenusEngine(ID);
        }
        //-----------------------------------------------------------------------------------------------



        //-- Transform-----------------------------------------------------------------------------------
        public Vector3 Position
        {
            get
            {
                return GetComponent<TransformComponent>().Position;
            }

            set
            {
                GetComponent<TransformComponent>().Position = value;
            }
        }

        public Vector3 Rotation
        {
            get
            {
                return GetComponent<TransformComponent>().Rotation;
            }

            set
            {
                GetComponent<TransformComponent>().Rotation = value;
            }
        }

        public Vector3 Scale
        {
            get
            {
                return GetComponent<TransformComponent>().Scale;
            }

            set
            {
                GetComponent<TransformComponent>().Scale = value;
            }
        }
        //-----------------------------------------------------------------------------------------------



        //-- Components----------------------------------------------------------------------------------
        public T AddComponent<T>() where T : Component, new()
        {
            AddComponent_VenusEngine(ID, typeof(T));
            T component = new T();
            component.Entity = this;
            return component;
        }

        public bool HasComponent<T>() where T : Component, new()
        {
            return HasComponent_VenusEngine(ID, typeof(T));
        }

        /*
        public bool HasComponent(Type type)
        {
            return HasComponent_VenusEngine(ID, type);
        }
        */

        public T GetComponent<T>() where T : Component, new()
        {
            if(HasComponent<T>())
            {
                T component = new T();
                component.Entity = this;
                return component;
            }

            return null;
        }
        //-----------------------------------------------------------------------------------------------



        //-- VenusEngine---------------------------------------------------------------------------------
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ulong CreateEntity_VenusEngine();
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void DestroyEntity_VenusEngine(ulong entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ulong FindEntityByName_VenusEngine(string name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool ExistEntity_VenusEngine(ulong entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern ulong GetParent_VenusEngine(ulong entityID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetParent_VenusEngine(ulong childID, ulong parentID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool HasParent_VenusEngine(ulong childID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern Entity[] GetChildren_VenusEngine(ulong entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void AddComponent_VenusEngine(ulong entityID, Type type);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool HasComponent_VenusEngine(ulong entityID, Type type);
    }
}
