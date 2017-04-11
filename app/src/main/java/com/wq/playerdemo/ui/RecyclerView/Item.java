package com.wq.playerdemo.ui.RecyclerView;

public class Item {
    private String URL;
    private String name;

    public Item(){
        URL = null;
        name = null;
    }

    public Item(String url, String name){
        this.URL = url;
        this.name = name;
    }

    public String getURL() {
        return URL;
    }

    public void setURL(String URL) {
        this.URL = URL;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }
}
