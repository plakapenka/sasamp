package com.sasamp.cr.gui.donate;

public class DonateItem {
    int imgRecourse;
    int price;
    int category;
    int value;
    String name;

    public DonateItem(String name, int category, int price, int imgRecourse, int value)
    {
        this.category = category;
        this.imgRecourse = imgRecourse;
        this.price = price;
        this.name = name;
        this.value = value;
    }
}
