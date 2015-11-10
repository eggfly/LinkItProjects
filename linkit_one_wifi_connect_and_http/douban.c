#include <stdio.h>
#include "douban.h"
#include "parson/parson.h"

/*
{
    r: 0,
    is_show_quick_start: 0,
    song: [{
        status: 0,
        picture: "http://img3.doubanio.com/lpic/s4704559.jpg",
        alert_msg: "",
        albumtitle: "会跳舞的文艺青年",
        promo: {},
        singers: [{
            related_site_id: 242313,
            is_site_artist: false,
            id: "3203",
            name: "李宇春"
        }],
        file_ext: "mp4",
        like: 0,
        album: "/subject/6097549/",
        ver: 0,
        ssid: "e06a",
        title: "淹死的鱼",
        url: "http://mr7.doubanio.com/32117f7d16ca9ff2b288c899e35d6361/0/fm/song/p1675423_128k.mp4",
        artist: "李宇春",
        subtype: "",
        length: 262,
        sid: "1675423",
        aid: "6097549",
        sha256: "b379dbd13e1418257b0552108ae5ddaee8fbe4da5b13bef115869784753bfc77",
        kbps: "128"
    }]
}
*/

int test(){
    const char* json = "{\"r\":0,\"is_show_quick_start\":0,\"song\":[{\"status\":0,\"picture\":\"http:\\/\\/img3.doubanio.com\\/lpic\\/s4704559.jpg\",\"alert_msg\":\"\",\"albumtitle\":\"会跳舞的文艺青年\",\"promo\":{},\"singers\":[{\"related_site_id\":242313,\"is_site_artist\":false,\"id\":\"3203\",\"name\":\"李宇春\"}],\"file_ext\":\"mp4\",\"like\":0,\"album\":\"\\/subject\\/6097549\\/\",\"ver\":0,\"ssid\":\"e06a\",\"title\":\"淹死的鱼\",\"url\":\"http:\\/\\/mr7.doubanio.com\\/32117f7d16ca9ff2b288c899e35d6361\\/0\\/fm\\/song\\/p1675423_128k.mp4\",\"artist\":\"李宇春\",\"subtype\":\"\",\"length\":262,\"sid\":\"1675423\",\"aid\":\"6097549\",\"sha256\":\"b379dbd13e1418257b0552108ae5ddaee8fbe4da5b13bef115869784753bfc77\",\"kbps\":\"128\"}]}";
    parse(json);
    return 0;
}

void parse(const char *json) {
    JSON_Value *root_value;

    root_value = json_parse_string(json);
    if (json_value_get_type(root_value) != JSONObject) {
        printf("error");
        return;
    }

    JSON_Object *obj = json_value_get_object(root_value);
    double r = json_object_get_number(obj, "r");
    JSON_Array *songs = json_object_get_array(obj, "song");
    size_t i;
    for (i = 0; i < json_array_get_count(songs); i++) {
        JSON_Object *song = json_array_get_object(songs, i);
        printf("albumtitle: %s\nartist: %s\n\n", json_object_get_string(song, "albumtitle"), json_object_get_string(song, "artist"));
    }
    // commits = json_value_get_array(root_value);
    // printf("%-10.10s %-10.10s %s\n", "Date", "SHA", "Author");
    // for (i = 0; i < json_array_get_count(commits); i++) {
    //     commit = json_array_get_object(commits, i);
    //     printf("%.10s %.10s %s\n",
    //           json_object_dotget_string(commit, "commit.author.date"),
    //           json_object_get_string(commit, "sha"),
    //           json_object_dotget_string(commit, "commit.author.name"));
    // }
    printf("r=%f", r);
    json_value_free(root_value);
}
