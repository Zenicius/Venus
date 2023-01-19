using System;
using System.Runtime.CompilerServices;

namespace Venus
{
    public abstract class Component
    {
        public Entity Entity { get; set; }
    }

    // TagComponent
    public class TagComponent : Component
    {
        public string Name
        {
            get
            {
                return GetEntityName_VenusEngine(Entity.ID);
            }

            set
            {
                SetEntityName_VenusEngine(Entity.ID, value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetEntityName_VenusEngine(ulong entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetEntityName_VenusEngine(ulong entityID, string name);
    }

    // TransformComponent
    public class TransformComponent : Component
    {
        // Relative to parent 
        public Transform Transform
        {
            get
            {
                GetTransform_VenusEngine(Entity.ID, out Transform transform);
                return transform;
            }

            set
            {
                SetTransform_VenusEngine(Entity.ID, ref value);
            }
        }
        
        // World coordinate space
        public Transform WorldTransform
        {
            get
            {
                GetWorldTransform_VenusEngine(Entity.ID, out Transform transform);
                return transform;
            }
        }

        // Position
        public Vector3 Position
        {
            get
            {
                GetPosition_VenusEngine(Entity.ID, out Vector3 position);
                return position;
            }

            set
            {
                SetPosition_VenusEngine(Entity.ID, ref value);
            }
        }

        // Rotation
        public Vector3 Rotation
        {
            get
            {
                GetRotation_VenusEngine(Entity.ID, out Vector3 rotation);
                return rotation;
            }

            set
            {
                SetRotation_VenusEngine(Entity.ID, ref value);
            }
        }

        // Scale
        public Vector3 Scale
        {
            get
            {
                GetScale_VenusEngine(Entity.ID, out Vector3 scale);
                return scale;
            }

            set
            {
                SetScale_VenusEngine(Entity.ID, ref value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetTransform_VenusEngine(ulong entityID, out Transform transform);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetTransform_VenusEngine(ulong entityID, ref Transform transform);


        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetWorldTransform_VenusEngine(ulong entityID, out Transform transform);


        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetPosition_VenusEngine(ulong entityID, out Vector3 position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetPosition_VenusEngine(ulong entityID, ref Vector3 position);


        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetRotation_VenusEngine(ulong entityID, out Vector3 rotation);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRotation_VenusEngine(ulong entityID, ref Vector3 rotation);


        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetScale_VenusEngine(ulong entityID, out Vector3 scale);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetScale_VenusEngine(ulong entityID, ref Vector3 scale);
    }

    // CameraComponent
    public class CameraComponent : Component
    {
        public bool Primary
        {
            get
            {
                return GetIsPrimary_VenusEngine(Entity.ID);
            }

            set
            {
                SetIsPrimary_VenusEngine(Entity.ID, value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool GetIsPrimary_VenusEngine(ulong entityID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetIsPrimary_VenusEngine(ulong entityID, bool value);
    }


    // PointLightComponent
    public class PointLightComponent : Component
    {
        public Vector3 Color
        {
            get
            {
                GetColor_VenusEngine(Entity.ID, out Vector3 color);
                return color;
            }

            set
            {
                SetColor_VenusEngine(Entity.ID, ref value);
            }
        }

        public float Intensity
        {
            get
            {
                return GetIntensity_VenusEngine(Entity.ID);
            }

            set
            {
                SetIntensity_VenusEngine(Entity.ID, value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetColor_VenusEngine(ulong entityID, out Vector3 color);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetColor_VenusEngine(ulong entityID, ref Vector3 color);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern float GetIntensity_VenusEngine(ulong entityID);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetIntensity_VenusEngine(ulong entityID, float intensity);
    }

    // RigidBody 2D
    public class RigidBody2DComponent : Component
    {
        public Vector2 Position
        {
            get
            {
                GetRb2DPosition_VenusEngine(Entity.ID, out Vector2 position);
                return position;
            }

            set
            {
                SetRb2DPosition_VenusEngine(Entity.ID, ref value);
            }
        }

        public Vector2 Velocity
        {
            get
            {
                GetRb2DVelocity_VenusEngine(Entity.ID, out Vector2 velocity);
                return velocity;
            }

            set
            {
                SetRb2DVelocity_VenusEngine(Entity.ID, ref value);
            }
        }

        public void ApplyLinearImpulse(Vector2 impulse, bool wake)
        {
            ApplyLinearImpulse_VenusEngine(Entity.ID, ref impulse, wake);    
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetRb2DPosition_VenusEngine(ulong entityID, out Vector2 position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRb2DPosition_VenusEngine(ulong entityID, ref Vector2 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void GetRb2DVelocity_VenusEngine(ulong entityID, out Vector2 velocity);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void SetRb2DVelocity_VenusEngine(ulong entityID, ref Vector2 velocity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void ApplyLinearImpulse_VenusEngine(ulong entityID, ref Vector2 impulse, bool wake);
    }
}
