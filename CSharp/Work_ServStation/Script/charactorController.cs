using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class charactorController : MonoBehaviour
{
    #region public members
//#if UNITY_EDITOR || UNITY_STANDALONE_WIN //PC
    [System.Serializable]
    public struct MotionPair
    {
       public string motionName;
       public KeyCode keyCode;
    }
    public MotionPair[] motionList;

    public static int motionLayerIndex = 0;

    public string WSServerAttachName = "LipSyncInterface";
    private WSServer ws_net = null;
    //#endif
    //public SkinnedMeshRenderer ref_SMR_EYE_DEF; //EYE_DEFへの参照
    //public SkinnedMeshRenderer ref_SMR_EL_DEF;  //EL_DEFへの参照
    //
    //[Tooltip("close rate")]
    //public float ratio_Close = 85.0f;           //閉じ目ブレンドシェイプ比率
    //[Tooltip("half close rate")]
    //public float ratio_HalfClose = 20.0f;       //半閉じ目ブレンドシェイプ比率
    //[Tooltip("open rate")]
    //public float ratio_Open = 0.0f;
    //OVRNamedArray motions = new OVRNamedArray(new string[] { "Rest", "Jump", "Kick", "Punch" }) ;
    //static string[] motions = new string[] { "Rest", "Jump", "Kick", "Punch" };

    #endregion

    #region private members
    //private Dictionary<string, KeyCode> motionDict = new Dictionary<string, KeyCode> {
    //    {"Rest",KeyCode.R },
    //    {"Jump",KeyCode.J },
    //    {"Kick",KeyCode.K },
    //    {"Punch",KeyCode.P }
    //};
    private Animator anim = null;                      // Animator
    private AnimatorStateInfo currentState;     // 当前状态（肢体动作部分）
    private AnimatorStateInfo previousState;    // 保存上一状态的引用（肢体动作部分）
    private float speed = 0.0f;
    private float direction = 0.0f;
    private float threshold = 0.3f;

    private float intervalRot = 1f;
    private float moveSpeed = 1.5f;
    private float ratio = 0.2f;
    #endregion

    //enum Status
    //{
    //    Close,
    //    HalfClose,
    //    Open    //目パチの状態
    //}
    //private Status eyeStatus;	//現在の目パチステータス
    // Start is called before the first frame update
    void Start()
    {
       // motionDict.Clear();
        anim = GetComponent<Animator>();
        if(anim != null)
        {
            currentState = anim.GetCurrentAnimatorStateInfo(motionLayerIndex);//motion layer
            previousState = currentState;
        }
        ws_net = GameObject.Find(WSServerAttachName).GetComponent<WSServer>();
        if (ws_net != null)
        {
            ws_net.customerImgDelegate += this.customerImgHandle;
        }

        StartCoroutine("MoveCoroutine");
    }

    // Update is called once per frame
    void Update()
    {
        if(anim == null)
        {
            return;
        }

        anim.SetLayerWeight(motionLayerIndex, 1.0f);
        currentState = anim.GetCurrentAnimatorStateInfo(motionLayerIndex);
        direction += ratio * (-direction);
        speed += ratio * (-speed);
        if(Mathf.Abs(speed) < 0.01 && Mathf.Abs(speed)>0.0001 * ratio)
        {
            anim.SetFloat("Speed", speed);
        }
        if (Mathf.Abs(direction) < 0.01 && Mathf.Abs(direction) > 0.0001 * ratio)
        {
            anim.SetFloat("Direction", direction);
        }

#if UNITY_EDITOR || UNITY_STANDALONE_WIN //PC

        if (Input.GetKey(KeyCode.W) || Input.GetKey(KeyCode.UpArrow))
        {
            // to move forward
            speed = 0.3f;
            anim.SetFloat("Speed", speed);
            
        }
        if(Input.GetKey(KeyCode.S) || Input.GetKey(KeyCode.DownArrow))
        {
            // to move backward
            speed = -0.3f;
            anim.SetFloat("Speed", speed);
        }
        if(Input.GetKey(KeyCode.A) || Input.GetKey(KeyCode.LeftArrow))
        {
            //to turn left
            direction = 0.3f;
            anim.SetFloat("Direction", direction);
        }
        if(Input.GetKey(KeyCode.D) || Input.GetKey(KeyCode.RightArrow))
        {
            //to turn right
            direction = -0.3f;
            anim.SetFloat("Direction", direction);
        }

        foreach(var motionPair in motionList)
        {
            if(anim.GetBool(motionPair.motionName))
                anim.SetBool(motionPair.motionName, false);
            if (Input.GetKey(motionPair.keyCode) && (!currentState.IsName(motionPair.motionName)))
            {
                //  to rest 
                anim.SetBool(motionPair.motionName, true);
            }
        }

        //if (anim.GetBool("Rest"))
        //    anim.SetBool("Rest", false);
        //if (Input.GetKey(KeyCode.R)&& (!currentState.IsName("Rest")))
        //{
        //    //  to rest 
        //    anim.SetBool("Rest", true);
        //}
        //
        //if(Input.GetKey(KeyCode.I))
        //{
        //    // attack pose 1
        //}
        //if(anim.GetBool("Jump"))
        //    anim.SetBool("Jump", false);
        //if (Input.GetKey(KeyCode.J)&& (!currentState.IsName("Jump")))
        //{
        //    // jump
        //    anim.SetBool("Jump", true);
        //}
        //
        //if (anim.GetBool("Kick"))
        //    anim.SetBool("Kick", false);
        //if (Input.GetKey(KeyCode.K) && (!currentState.IsName("Kick")))
        //{
        //    // Kick 踢腿
        //    anim.SetBool("Kick", true);
        //}
        //if (Input.GetKey(KeyCode.L))
        //{
        //    // attack pose 4
        //}


#endif
    }

    private void customerImgHandle(NetBase.PeopleImg pi)
    {
        //Service currentService = getService(pi.FACE_DATA.action);
        //serviceHandle(currentService);
        if (anim == null)
        {
            return;
        }
        UnityMainThreadDispatcher.Instance().Enqueue(customerImgCoroutine(pi));
    }

    IEnumerator customerImgCoroutine(NetBase.PeopleImg pi)
    {
        anim.SetLayerWeight(motionLayerIndex, 1.0f);
        currentState = anim.GetCurrentAnimatorStateInfo(motionLayerIndex);
        foreach (var motionPair in motionList)
        {
            if (anim.GetBool(motionPair.motionName))
                anim.SetBool(motionPair.motionName, false);
            if ((motionPair.motionName == pi.FACE_DATA.action) && (!currentState.IsName(motionPair.motionName)))
            {
                //  to rest 
                anim.SetBool(motionPair.motionName, true);
            }
        }
        yield return null;
    }

    // void LateUpdate()
    // {
    //     setEye();
    // }
    // 
    // void setEye()
    // {
    //     switch (eyeStatus)
    //     {
    //         case Status.Close:
    //             SetCloseEyes();
    //             break;
    //         case Status.HalfClose:
    //             SetHalfCloseEyes();
    //             break;
    //         case Status.Open:
    //             SetOpenEyes();
    //             break;
    //     }
    // }
    // 
    // void SetCloseEyes()
    // {
    //     ref_SMR_EYE_DEF.SetBlendShapeWeight(6, ratio_Close);
    //     ref_SMR_EL_DEF.SetBlendShapeWeight(6, ratio_Close);
    // }
    // 
    // void SetHalfCloseEyes()
    // {
    //     ref_SMR_EYE_DEF.SetBlendShapeWeight(6, ratio_HalfClose);
    //     ref_SMR_EL_DEF.SetBlendShapeWeight(6, ratio_HalfClose);
    // }
    // 
    // void SetOpenEyes()
    // {
    //     ref_SMR_EYE_DEF.SetBlendShapeWeight(6, ratio_Open);
    //     ref_SMR_EL_DEF.SetBlendShapeWeight(6, ratio_Open);
    // }

    IEnumerator MoveCoroutine()
    {
        while(true)
        {
            BodyMove();
            yield return null;
        }
    }
    void BodyMove()
    {
        currentState = anim.GetCurrentAnimatorStateInfo(motionLayerIndex);
        var tranformValue = new Vector3();
        float rotValue = 0;
        if (currentState.IsName("WalkForward"))
        {
            // to move forward
            tranformValue = Vector3.forward * Time.deltaTime;
            //anim.SetFloat("Speed", 0);
        }
        if (currentState.IsName("WalkBack"))
        {
            // to move backward
            tranformValue = -Vector3.forward * Time.deltaTime;
            //anim.SetFloat("Speed", 0);
        }
        if(currentState.IsName("WalkLeft"))
        {
            // move left
            //tranformValue = Vector3.left * Time.deltaTime;
            rotValue = -intervalRot;
            //anim.SetFloat("Direction", 0);
        }
        if (currentState.IsName("WalkRight"))
        {
            // move right
            rotValue = intervalRot;
            //anim.SetFloat("Direction", 0);
            //tranformValue = -Vector3.left * Time.deltaTime;
        }
        transform.Rotate(Vector3.up, rotValue);
        transform.Translate(tranformValue * moveSpeed);
        previousState = currentState;
    }
}
