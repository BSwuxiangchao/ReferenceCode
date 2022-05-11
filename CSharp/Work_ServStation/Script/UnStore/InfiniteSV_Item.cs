using System.Security.Cryptography.X509Certificates;
using System.Net.Mime;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

/// <summary>
/// 元素携带数据
/// </summary>
public class InfiniteSV_ItemData
{
    public string ItemImageName = "";
    public string ItemInfo = "";

    public InfiniteSV_ItemData(string itemImageName, string itemInfo)
    {
        ItemImageName = itemImageName;
        ItemInfo = itemInfo;
    }
}
public class InfiniteSV_Item : MonoBehaviour, IPointerClickHandler
{
    public int _itemIndex = 0;
    public RectTransform _itemRect;
    public Image _itemImage;
    public Text _itemText;
    public InfiniteSV_ItemData _itemData;
    public InfiniteSV _infiniteSV;
    public void InitItem(InfiniteSV _targetInfiniteSV, string itemImageName, string itmeInfo = "")
    {
        _infiniteSV = _targetInfiniteSV;
        _itemRect = GetComponent<RectTransform>();
        _itemImage = GetComponent<Image>();
        _itemText = transform.Find("Text").GetComponent<Text>();
        _itemData = new InfiniteSV_ItemData(itemImageName, itmeInfo);
        InitData();
    }
    public void InitData()
    {
        _itemImage.sprite = Resources.Load<Sprite>("Images/Unstore/Foods/" + _itemData.ItemImageName);
        _itemText.text = _itemData.ItemImageName;
    }
    public void UpadatData(InfiniteSV_ItemData _infiniteSV_ItemData)
    {
        _itemData = _infiniteSV_ItemData;
        InitData();

    }


    public void OnPointerClick(PointerEventData eventData)
    {
        _infiniteSV.SetCenterItemByItemPos(transform.localPosition);
    }
}