using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace LightBuzz.Kinect4Azure
{
    public class VtuberMapJoint : MonoBehaviour
    {
        // Start is called before the first frame update
        public GameObject head;

        public GameObject leftArm;
        public GameObject leftForeArm;
        public GameObject leftHand;

        public GameObject rightArm;
        public GameObject rightForeArm;
        public GameObject rightHand;

        public Vector3D sensor2WorldZ = new Vector3D(180.0f, 0.0f, 0.0f);

        public void updateBody(Body body)
        {
            //var body.Joints[JointType.Neck].Orientation;
            foreach(var pair in body.Joints)
            {
                switch(pair.Key)
                {
                    case JointType.Head:
                        setHeadOrientation(head, pair.Value);
                        break;
                    case JointType.ShoulderLeft:
                        setLeft(leftArm, pair.Value);
                        break;
                    case JointType.ElbowLeft:
                        setLeft(leftForeArm, pair.Value);
                        break;
                    case JointType.HandLeft:
                        setLeftHand(leftHand, pair.Value);
                        break;
                    case JointType.ShoulderRight:
                        setRight(rightArm, pair.Value);
                        break;
                    case JointType.ElbowRight:
                        setRight(rightForeArm, pair.Value);
                        break;
                    case JointType.HandRight:
                        setRightHand(rightHand, pair.Value);
                        break;
                }
            }
        }

        private Vector3D joint2world(Vector4D orientation)
        {
            var j2s = new Quaternion(orientation.X, orientation.Y, orientation.Z, orientation.W);

            var s2w = Quaternion.Euler(sensor2WorldZ);
            var world = s2w * j2s;
            return world.EulerAngles;
        }
        private void setLeft(GameObject obj, Joint j)
        {
            if (obj == null)
                return;
            var q = getJoint2Camera(j);
            var jh = q * Quaternion.Euler(0, 180, 0);
            //var jh = q * Quaternion.Euler(0, 180, 0);
            var e = joint2world(new Vector4D(jh.X,jh.Y,jh.Z,jh.W));
            obj.transform.eulerAngles = new Vector3(e.X, e.Y, e.Z);

            //obj.transform.localRotation = new Quaternion(j.Orientation.X, j.Orientation.Y, j.Orientation.Z, j.Orientation.W);
        }

        private void setLeftHand(GameObject obj, Joint j)
        {
            if (obj == null)
                return;

            var q = getJoint2Camera(j);
            //var jh = Quaternion.Euler(0,180, 0)* q;
            var jh = q * Quaternion.Euler(-90, 180, 0);
            var e = joint2world(new Vector4D(jh.X, jh.Y, jh.Z, jh.W));
            obj.transform.eulerAngles = new Vector3(e.X, e.Y, e.Z);

            //obj.transform.localRotation = new Quaternion(j.Orientation.X, j.Orientation.Y, j.Orientation.Z, j.Orientation.W);
        }

        private void setRight(GameObject obj, Joint j)
        {
            if (obj == null)
                return;
            var q = getJoint2Camera(j);
            //var jh = Quaternion.Euler(180,0, 0)* q;
            var jh = q * Quaternion.Euler(180, 0, 0);
            var e = joint2world(new Vector4D(jh.X, jh.Y, jh.Z, jh.W));
            obj.transform.eulerAngles = new Vector3(e.X, e.Y, e.Z);
            //obj.transform.localRotation = new Quaternion(j.Orientation.X, j.Orientation.Y, j.Orientation.Z, j.Orientation.W);
        }

        private void setRightHand(GameObject obj, Joint j)
        {
            if (obj == null)
                return;
            var q = getJoint2Camera(j);
            //var jh = Quaternion.Euler(180,0, 0)* q;
            var jh = q * Quaternion.Euler(180, 0, 0);
            var e = joint2world(new Vector4D(jh.X, jh.Y, jh.Z, jh.W));
            obj.transform.eulerAngles = new Vector3(e.X, e.Y, e.Z);
            //obj.transform.localRotation = new Quaternion(j.Orientation.X, j.Orientation.Y, j.Orientation.Z, j.Orientation.W);
        }

        private void setHeadOrientation(GameObject obj, Joint j)
        {
            if (obj == null)
                return;
            var q = getJoint2Camera(j);
            var jh = q * Quaternion.Euler(0, 180, 0);
            var e = joint2world(new Vector4D(jh.X, jh.Y, jh.Z, jh.W));
            obj.transform.eulerAngles = new Vector3(e.X, e.Y, e.Z);
        }

        private Quaternion getJoint2Camera(Joint j)
        {
            var q = new Quaternion(j.Orientation.X, j.Orientation.Y, j.Orientation.Z, j.Orientation.W);
            //var jh = q * Quaternion.Euler(0, 180, 0);
            var Qtmp = Quaternion.Euler(-q.EulerAngles.Z,-q.EulerAngles.Y,-q.EulerAngles.X);
            return q;
        }
    }

}
