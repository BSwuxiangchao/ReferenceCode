using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FaceController : MonoBehaviour
{
    #region public members
    //#if UNITY_EDITOR || UNITY_STANDALONE_WIN //PC
    [System.Serializable]
    public struct FaceEmotionPair
    {
        public string emotionName;
        public KeyCode keyCode;
    }
    public FaceEmotionPair[] emotionList;

    public int faceLayerIndex = 1;

    public string WSServerAttachName = "LipSyncInterface";
    private WSServer ws_net = null;

    #region Face attr

    public SkinnedMeshRenderer ref_SMR_EYE_DEF; //眼皮引用
    public SkinnedMeshRenderer ref_SMR_EL_DEF;	//眼线眼睫毛引用
    public SkinnedMeshRenderer ref_SMR_MTH_DEF; //嘴引用

    public GameObject head;

    public GameObject leftArm;
    public GameObject leftForeArm;
    public GameObject leftHand;

    public GameObject rightArm;
    public GameObject rightForeArm;
    public GameObject rightHand;
    #endregion


    //#endif
    #endregion
    //    #region public members
    //    public SkinnedMeshRenderer ref_SMR_EYE_DEF; //EYE_DEFへの参照
    //    public SkinnedMeshRenderer ref_SMR_EL_DEF;  //EL_DEFへの参照
    //
    //    [Tooltip("close rate")]
    //    public float ratio_Close = 85.0f;           //閉じ目ブレンドシェイプ比率
    //    [Tooltip("half close rate")]
    //    public float ratio_HalfClose = 20.0f;       //半閉じ目ブレンドシェイプ比率
    //    [Tooltip("open rate")]
    //    public float ratio_Open = 0.0f;
    //    #endregion
    //
    //
    //
    //    enum Status
    //    {
    //        Close,
    //        HalfClose,
    //        Open    //目パチの状態
    //    }
    //    private Status eyeStatus;	//現在の目パチステータス
    //    // Start is called before the first frame update
    //    void Start()
    //    {
    //
    //    }
    //
    //    // Update is called once per frame
    //    void Update()
    //    {
    //#if UNITY_EDITOR || UNITY_STANDALONE_WIN //PC
    //
    //        if (Input.GetKey(KeyCode.C))
    //        {
    //            // to close eye
    //            eyeStatus = Status.Close;
    //        }
    //        if (Input.GetKey(KeyCode.H))
    //        {
    //            //to half close eye
    //            eyeStatus = Status.HalfClose;
    //        }
    //        if (Input.GetKey(KeyCode.O))
    //        {
    //            //to open eye
    //            eyeStatus = Status.Open;
    //        }
    //        //setEye();
    //#endif
    //    }
    //
    //    void LateUpdate()
    //    {
    //        setEye();
    //    }
    //
    //    void setEye()
    //    {
    //        switch (eyeStatus)
    //        {
    //            case Status.Close:
    //                SetCloseEyes();
    //                break;
    //            case Status.HalfClose:
    //                SetHalfCloseEyes();
    //                break;
    //            case Status.Open:
    //                SetOpenEyes();
    //                break;
    //        }
    //    }
    //
    //    void SetCloseEyes()
    //    {
    //        ref_SMR_EYE_DEF.SetBlendShapeWeight(6, ratio_Close);
    //        ref_SMR_EL_DEF.SetBlendShapeWeight(6, ratio_Close);
    //    }
    //
    //    void SetHalfCloseEyes()
    //    {
    //        ref_SMR_EYE_DEF.SetBlendShapeWeight(6, ratio_HalfClose);
    //        ref_SMR_EL_DEF.SetBlendShapeWeight(6, ratio_HalfClose);
    //    }
    //
    //    void SetOpenEyes()
    //    {
    //        ref_SMR_EYE_DEF.SetBlendShapeWeight(6, ratio_Open);
    //        ref_SMR_EL_DEF.SetBlendShapeWeight(6, ratio_Open);
    //    }


    #region private members
    private Animator anim = null;                      // Animator
    private AnimatorStateInfo currentState;     // 当前状态（肢体动作部分）
    private AnimatorStateInfo previousState;    // 保存上一状态的引用（肢体动作部分）
    #endregion
    void Start()
    {
        // motionDict.Clear();
        anim = GetComponent<Animator>();
        if (anim != null)
        {
            currentState = anim.GetCurrentAnimatorStateInfo(faceLayerIndex);//face layer
            previousState = currentState;
        }
        // StartCoroutine("MoveCoroutine");
        ws_net = GameObject.Find(WSServerAttachName).GetComponent<WSServer>();
        if (ws_net != null)
        {
            ws_net.customerImgDelegate += this.customerImgHandle;
            ws_net.vtuberDelegate += this.vtuberImgHandle;
        }
        //SetObjEulerAngle(head, new Vector3(0,45,0));
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
        anim.SetLayerWeight(faceLayerIndex, 1.0f);
        currentState = anim.GetCurrentAnimatorStateInfo(faceLayerIndex);
        foreach (var motionPair in emotionList)
        {
            if ((pi.FACE_DATA.emotion == motionPair.emotionName) && (!currentState.IsName(motionPair.emotionName)))
            {
                anim.CrossFade(motionPair.emotionName, 0.0f, faceLayerIndex);
            }
        }
        yield return null;
    }

    private void vtuberImgHandle(NetBase.VtuberImg vi)
    {
        //Service currentService = getService(pi.FACE_DATA.action);
        //serviceHandle(currentService);

        UnityMainThreadDispatcher.Instance().Enqueue(vtuberImgCoroutine(vi));
    }
    IEnumerator vtuberImgCoroutine(NetBase.VtuberImg vi)
    {
        // to do face data binding
        float ratioEye = 0.0f;
        ratioEye = 100 * (2 - vi.IMG_DATA.faceData.eyeL - vi.IMG_DATA.faceData.eyeR) / 2.0f;
        ref_SMR_EYE_DEF?.SetBlendShapeWeight(6, ratioEye);
        ref_SMR_EL_DEF?.SetBlendShapeWeight(6, ratioEye);
        float ratioMth = vi.IMG_DATA.faceData.mouth * 100;
        ref_SMR_MTH_DEF?.SetBlendShapeWeight(6, ratioMth);
        //to do body skelon data binding
        //head
        //var headTransf = head?.gameObject.transform;
        //Vector3 headAngle = new Vector3(vi.IMG_DATA.head.X, vi.IMG_DATA.head.Y, vi.IMG_DATA.head.Z);
        //headTransf.eulerAngles = headAngle;
        // SetObjEulerAngle(head, vi.IMG_DATA.head);
        // SetObjEulerAngle(leftArm, vi.IMG_DATA.armL);
        // SetObjEulerAngle(leftForeArm, vi.IMG_DATA.fore_armL);
        // SetObjEulerAngle(leftHand, vi.IMG_DATA.handL);
        // 
        // SetObjEulerAngle(rightArm, vi.IMG_DATA.armR);
        // SetObjEulerAngle(rightForeArm, vi.IMG_DATA.fore_armR);
        // SetObjEulerAngle(rightHand, vi.IMG_DATA.handR);
        yield return null;
    }

    private void SetObjEulerAngle(GameObject obj, NetBase.EulerAngle euler)
    {
        var transf = obj?.gameObject.transform;
        Vector3 eulerAngle = new Vector3(euler.X, euler.Y, euler.Z);
        transf.localEulerAngles = eulerAngle;
    }

    private void SetObjEulerAngle(GameObject obj, Vector3 euler)
    {
        var transf = obj?.gameObject.transform;
        //transf.eulerAngles = euler;
        transf.localEulerAngles = euler;
    }

    void Update()
    {
        if (anim == null)
        {
            return;
        }
        anim.SetLayerWeight(faceLayerIndex, 1.0f);
        currentState = anim.GetCurrentAnimatorStateInfo(faceLayerIndex);
        foreach (var motionPair in emotionList)
        {
            if (Input.GetKey(motionPair.keyCode) && (!currentState.IsName(motionPair.emotionName)))
            {
                //  to rest 
                anim.CrossFade(motionPair.emotionName, 0.0f, faceLayerIndex);
            }
        }
    }

}
