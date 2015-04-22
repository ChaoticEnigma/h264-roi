(function(){
    var script = document.createElement("script");
    script.src = "https://ajax.googleapis.com/ajax/libs/jquery/2.1.3/jquery.min.js";
    script.type = 'text/javascript';
    document.getElementsByTagName("head")[0].appendChild(script);

    var checkReady = function(callback){
        if(window.jQuery){
            callback(jQuery);
        } else {
            window.setTimeout(function(){
                checkReady(callback);
            }, 100);
        }
    };

    var sendSave = function(record){
        var outrecord = {};
        if(record.id)               outrecord.id        = record.id;
        if(record.score)            outrecord.score     = record.score;
        if(record.parent)           outrecord.parent    = record.parent;
        if(record.artistid)         outrecord.artistid  = record.artistid;
        if(record.artist)           outrecord.artist    = record.artist;
        if(record.title)            outrecord.title     = record.title;
        if(record.category)         outrecord.category  = record.category;
        if(record.tags)             outrecord.tags      = record.tags;
        if(record.character)        outrecord.character = record.character;
        if(record.alltags)          outrecord.alltags   = record.alltags;
        if(record.url)              outrecord.url       = record.url;
        if(record.session)          outrecord.session   = record.session;
        if(record.type)             outrecord.type      = record.type;
        outrecord.site = record.site;

        var json = JSON.stringify(outrecord);

        var saveurl;
        if(typeof surl !== 'undefined'){
            saveurl = surl;
        } else {
            saveurl = "https://localhost/save.php";
        }
        
        var request = new XMLHttpRequest();
        if(request){
            request.open('POST', saveurl, true);
            request.onreadystatechange = function(){
                if(request.readyState === 4) {
                    if(request.status === 0 || request.status === 200){
                        console.log("success: " + request.status);
                    } else {
                        console.error("error: " + request.status);
                    }
                }
            };
            request.setRequestHeader("Content-Type", "text/plain");
            request.setRequestHeader("Content-Length", json.length);
            request.setRequestHeader("Connection", "close");
            request.send(json);
        }
    };

    var save_e621 = function($, record){
        record.site = "e621";

        // Get post ID
        var id = $("div#stats ul li:contains('ID:')").html();
        record.id = id.substring(id.indexOf(' ') + 1);

        // Get parent post ID
        var parentid = $("div.sidebar div.status-notice h6:contains('Parent Post') ~ p a").attr("href");
        if(parentid){
            record.parent = parentid.replace("/post/show/", "");
        }

        // Get image URL
        record.url = $("div#right-col h4 a:last").attr("href");

        // Get Artist tags
        record.artist = $("ul#tag-sidebar li.tag-type-artist a:nth-child(2)").map(function(){
            var str = $(this).attr("href");
            return unescape(str.substring(str.indexOf('=') + 1));
        }).get();

        // Get Character tags
        var characters = $("ul#tag-sidebar li.tag-type-character a:nth-child(2)").map(function(){
            var str = $(this).attr("href");
            return unescape(str.substring(str.indexOf('=') + 1));
        }).get();
        if(characters.length){
            record.character = characters;
        }

        // Custom search tags
        var findtags = ["breasts", "nipples", "pussy", "sex", "oral", "anal", "vaginal", "penetration", "male/female", "female/female"];

        // Get all other tags
        record.tags = [];
        record.alltags = $("ul#tag-sidebar li:not(.tag-type-artist,.tag-type-character) a:nth-child(2)").map(function(){
            var str = $(this).attr("href");
            var tag = str.substring(str.indexOf('=') + 1);
            if(findtags.indexOf(unescape(tag)) > -1)
                record.tags.push(unescape(tag))
            return unescape(tag);
        }).get();

        console.log(record.site + " - Score: " + record.score + ", ID: " + record.id);
        sendSave(record);
    };

    var save_konachan = function($, record){
        record.site = "konachan";

        // Get post ID
        var id = $("div#stats ul li:contains('Id:')").html();
        record.id = id.substring(id.indexOf(' ') + 1);

        // Get parent post ID
        var parentid = $("div#post-view div.status-notice a:contains('parent post')").attr("href");
        if(parentid){
            record.parent = parentid.replace("/post/show/", "");
        }

        // Get image URL
        record.url = $("div#post-view li a#highres").attr("href");

        // Get Artist tags
        record.artist = $("ul#tag-sidebar li.tag-type-artist a:nth-child(2)").map(function(){
            var str = $(this).attr("href");
            return unescape(str.substring(str.indexOf('=') + 1));
        }).get();

        // Get Character tags
        var characters = $("ul#tag-sidebar li.tag-type-character a:nth-child(2)").map(function(){
            var str = $(this).attr("href");
            return unescape(str.substring(str.indexOf('=') + 1));
        }).get();
        if(characters.length){
            record.character = characters;
        }

        // Custom search tags
        var findtags = ["censored", "uncensored", "breasts", "nipples", "pussy", "sex"];

        // Get all other tags
        record.tags = [];
        record.alltags = $("ul#tag-sidebar li:not(.tag-type-artist,.tag-type-character) a:nth-child(2)").map(function(){
            var str = $(this).attr("href");
            var tag = str.substring(str.indexOf('=') + 1);
            if(findtags.indexOf(unescape(tag)) > -1)
                record.tags.push(unescape(tag))
            return unescape(tag);
        }).get();

        console.log(record.site + " - Score: " + record.score + ", ID: " + record.id);
        sendSave(record);
    };

    var save_sankaku = function($, record){
        record.site = "sankaku";

        // Get post ID
        var id = $("h1#site-title a:nth-child(2)").attr("href");
        record.id = id.replace("/post/show/", "");

        // Get parent post ID
        var parentid = $("div#post-view div.status-notice a:contains('parent post')").attr("href");
        if(parentid){
            record.parent = parentid.replace("/post/show/", "");
        }

        // Get image URL
        record.url = "https:" + $("div#post-view li a#highres").attr("href");

        // Get Artist tags
        var artists = $("ul#tag-sidebar li.tag-type-artist > a").map(function(){
            var str = $(this).attr("href");
            return unescape(str.substring(str.indexOf('=') + 1));
        }).get();
        if(artists.length){
            record.artist = artists;
        }

        // Get Character tags
        var characters = $("ul#tag-sidebar li.tag-type-character > a").map(function(){
            var str = $(this).attr("href");
            return unescape(str.substring(str.indexOf('=') + 1));
        }).get();
        if(characters.length){
            record.character = characters;
        }

        // Custom search tags
        var findtags = ["censored", "uncensored", "cleavage", "breasts", "nipples", "vagina", "sex"];

        // Get all other tags
        record.tags = [];
        record.alltags = $("ul#tag-sidebar li:not(.tag-type-artist,.tag-type-character) > a").map(function(){
            var str = $(this).attr("href");
            var tag = str.substring(str.indexOf('=') + 1);
            if(findtags.indexOf(unescape(tag)) > -1)
                record.tags.push(unescape(tag))
            return unescape(tag);
        }).get();

        console.log(record.site + " - Score: " + record.score + ", ID: " + record.id);
        sendSave(record);
    };

    var save_pixiv = function($, record){
        record.site = "pixiv";

        // Get illustration ID
        record.id = location.href.substring(location.href.indexOf("illust_id=") + 10);

        // Get artist info
        var artisturl = $('div.profile-unit a.user-link').attr('href');
        record.artistid = artisturl.substring(artisturl.indexOf("id=") + 3);
        record.artist = $('div.profile-unit a.user-link h1.user').html();

        // Title
        record.title = $('div#wrapper h1.title').html();
        
        // Custom search tags
        var findtags = ["R-18"];

        // Get all other tags
        record.tags = [];
        record.alltags = $("section.work-tags span.tags-container ul.tags li.tag a.text").map(function(){
            var tag = $(this).html();
            if(findtags.indexOf(tag) > -1)
                record.tags.push(tag)
            return tag;
        }).get();

        // Get URL(s)
        if($("div.works_display div.ui-modal-trigger").size()){ // Modal Preview
            var url = $("div._illust_modal img.original-image").attr("data-src");
            record.url = [url];
            record.session = { "Referer" : location.href };
            record.type = "illustration";

            console.log(record.site + " - Type: " + record.type + ", Score: " + record.score + ", ID: " + record.id);
            sendSave(record);
        } else { // Manga Preview
            // Get preview page url
            var previewpath = $('div.works_display > a').attr("href");
            $.ajax({
                url: previewpath,
                crossDomain: true,
            }).done(function(data){
                var urls = $(data).find("div.item-container img").map(function(){
                    var url = $(this).attr("data-src");
                    return url;
                }).get();
                record.url = urls;
                record.session = { "Referer" : location.protocol + "//" + location.host + "/" + previewpath };
                record.type = "manga";

                console.log(record.site + " - Type: " + record.type + ", Score: " + record.score + ", ID: " + record.id);
                sendSave(record);
            });
        }
    };

    var save_deviantart = function($, record){
        record.site = "deviantart";

        var dlbutton = $("a.dev-page-button.dev-page-download");
        if(dlbutton.length){
            // ID
            record.id = dlbutton.attr("data-deviationid");
            // URL
            record.url = dlbutton.attr("href");
        } else {
            var img = $("div.dev-view-deviation img.dev-content-normal");
            // ID
            record.id = img.attr("data-embed-id");
            // URL
            record.url = img.attr("src");
        }

        // Artist
        record.artist = location.hostname.substring(0, location.hostname.indexOf("."));

        // Title
        record.title = $("div.dev-title-container > h1 > a").html();

        // Tags
        var tags = $("div.dev-title-container div.dev-about-breadcrumb a.discoverytag").map(function(){
            console.log($(this).attr("data-canonical-tag"));
            return $(this).attr("data-canonical-tag");
        }).get();
        if(tags.length){
            record.alltags = tags;
        }

        // Category
        var cat = $("div.dev-title-container span.dev-about-breadcrumb a.h").last().attr("href");
        cat = cat.substring(cat.indexOf("all/") + 4);
        cat = cat.substring(0, cat.length - 1);
        if(cat.length){
            record.category = cat;
        }

        // Chech for mature tag
        var mature = $("div.dev-metainfo-details dl dt:contains(Mature Content) ~ dd");
        if(mature.length && mature.html() === "Yes"){
            record.tags = [ "mature" ];
        }

        console.log(record.site + " - Score: " + record.score + ", ID: " + record.id);
        sendSave(record);

    };

    var save_furaffinity = function($, record){
        record.site = "furaffinity";

        // ID
        var id = location.pathname.substring(location.pathname.indexOf("view/") + 5);
        record.id = id.substring(0, id.length - 1);

        // URL
        record.url = "http:" + $("div.alt1.actions b a:contains(Download)").attr("href");

        // Title
        record.title = $("td.alt1 table.maintable > tbody > tr > td > b").html();

        // Artist
        var artist = $("td.alt1 table.maintable > tbody > tr a").attr("href");
        artist = artist.substring(artist.indexOf("user/") + 5);
        record.artist = artist.substring(0, artist.length - 1);

        // Search tags
        var findtags = ["boobs", "nipples", "pussy", "sex", "oral", "anal", "vaginal"];

        // Get keywords / tags
        record.tags = [];
        var tags = $("div#keywords a").map(function(){
            var tag = $(this).attr("href");
            tag = tag.substring(tag.indexOf(" ") + 1);
            if(findtags.indexOf(tag) > -1)
                record.tags.push(tag);
            return tag;
        }).get();
        if(tags.length){
            record.alltags = tags;
        }

        console.log(record.site + " - Score: " + record.score + ", ID: " + record.id);
        sendSave(record);
    };

    var save_paheal = function($, record){
        record.site = "paheal";

        // ID
        var id = location.pathname.substring(location.pathname.indexOf("view/") + 5);
        record.id = id.substring(0, id.indexOf("#"));

        // URL
        record.url = $("img#main_image").attr("src");

        // Tags
        record.tags = $("section#Tagsleft a.tag_name").map(function(){
            return $(this).html();
        }).get();

        console.log(record.site + " - Score: " + record.score + ", ID: " + record.id);
        sendSave(record);
    }

    checkReady(function($){
        var record = {};

        // Get Score user input:
        // 0: Unknown
        // 1: Poor
        // 2: Average + Downloaded Before
        // 3: Average
        // 4: Good
        // 5: Excellent
        if(typeof score !== 'undefined'){
            record.score = String(score);
        } else {
            record.score = String(0);
        }

        if(location.host === "e621.net"){
            save_e621($, record);
        } else if(location.host === "konachan.com"){
            save_konachan($, record);
        } else if(location.host === "chan.sankakucomplex.com"){
            save_sankaku($, record);
        } else if(location.host === "www.pixiv.net"){
            save_pixiv($, record);
        } else if(location.host.indexOf("deviantart.com") > 0){
            save_deviantart($, record);
        } else if(location.host === "www.furaffinity.net"){
            save_furaffinity($, record);
        } else if(location.host === "rule34.paheal.net"){
            save_paheal($, record);
        } else {
            console.error(location.host + " - Unsupported Site");
            record.site = "unknown";
            record.url = location.href;
            sendSave(record);
        }

    });
})();
