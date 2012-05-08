#include "emoticons.h"
#include "ui_emoticons.h"

struct emoticon
{
    char const *emotion;
    char const *gif;
};

static emoticon const emoticons[] = {
    { "(aaagh)",        "Aaagh"         },
    { "(admire)",       "Admire"        },
    { "(handclap)",     "Applause"      },
    { ";o",             "Arrogant"      },
    { "(basketball)",   "Basketball"    },
    { "(beckon)",       "Beckon"        },
    { "(beer)",         "Beer"          },
    { "--b",            "Blush"         },
    { "(bomb)",         "Bomb"          },
    { ">-|",            "Booo"          },
    { "(bow)",          "Bow"           },
    { "</3",            "BrokenHeart"   },
    { "(^)",            "Cake"          },
    { "8-)",            "Chill"         },
    { ";P",             "Chuckle"       },
    { "(cleaver)",      "Cleaver"       },
    { "(coffee)",       "Coffee"        },
    { "(?)",            "Confused"      },
    { ":+",             "Cool"          },
    { ":'(",            "Cry"           },
    { "(dagger)",       "Dagger"        },
    { "P-(",            "Distressed"    },
    { ";d",             "Doubtful"      },
    { ":B",             "Drooling"      },
    { "|-)",            "Drowsy"        },
    { ":-|",            "Embarassed"    },
    { "&>",             "FightingBoy"   },
    { "<&",             "FightingGirl"  },
    { "(fist)",         "Fist"          },
    { "&-(",            "Flushed"       },
    { ":(",             "Frown"         },
    { ":8",             "Frustrated"    },
    { ":~",             "Grimace"       },
    { ":D",             "Grin"          },
    { "(hammer)",       "Hammer"        },
    { "<3",             "Heart"         },
    { "B-)",            "Hellooo"       },
    { "(heyhey)",       "HeyHey"        },
    { "\\o/",           "Hooray"        },
    { "(hug)",          "Hug"           },
    { "@_@",            "Hypnotised"    },
    { "(L)",            "InLove"        },
    { ";!",             "InTheDark"     },
    { "(jump)",         "Jump"          },
    { "(K)",            "Kisses"        },
    { "(ladybug)",      "Ladybug"       },
    { ":>",             "Laugh"         },
    { "(lightning)",    "Lightning"     },
    { "(love)",         "Love"          },
    { "(lovebirds)",    "Lovebirds"     },
    { "(moon)",         "Moon"          },
    { "(no)",           "NoNo"          },
    { "(ok)",           "OK"            },
    { ":@",             "OnFire"        },
    { ":!",             "Panic"         },
    { "(dig)",          "PickNose"      },
    { "(pig)",          "Pig"           },
    { "(pingpong)",     "PingPong"      },
    { "(pinky)",        "Pinky"         },
    { ";-D",            "Pleased"       },
    { "(poop)",         "Poop"          },
    { "(present)",      "Present"       },
    { ":*",             "Pucker"        },
    { ":T",             "Puke"          },
    { "(wipe)",         "Relieved"      },
    { "(rice)",         "Rice"          },
    { "(rose)",         "Rose"          },
    { ":-S",            "Scold"         },
    { ":|",             "Scowl"         },
    { ":Q",             "Scream"        },
    { "(shake)",        "Shake"         },
    { ";x",             "Shhh"          },
    { ":$",             "Shy"           },
    { ":X",             "Silence"       },
    { "(sit)",          "Sit"           },
    { "(!!!)",          "Skull"         },
    { ":Z",             "Sleep"         },
    { "X-)",            "Sly"           },
    { ":)",             "Smile"         },
    { "(smooches)",     "Smooches"      },
    { ":g",             "Sneer"         },
    { ":'|",            "Sniffle"       },
    { "<@",             "Snub1"         },
    { "@>",             "Snub2"         },
    { ":<",             "Sob"           },
    { "(soccer)",       "Soccer"        },
    { ":;",             "Soldier"       },
    { ";f",             "Strive"        },
    { "(sun)",          "Sun"           },
    { ":O",             "Surprised"     },
    { "(surrender)",    "Surrender"     },
    { ":L",             "Sweating"      },
    { "(N)",            "ThumbsDown"    },
    { "(Y)",            "ThumbsUp"      },
    { "(turn)",         "TurnAway"      },
    { "(twirl)",        "Twirl"         },
    { "@x",             "UhOh"          },
    { "(victory)",      "Victory"       },
    { "(waddle)",       "Waddle"        },
    { "(melon)",        "WaterMelon"    },
    { "(wave)",         "Wave"          },
    { "8*",             "Whimper"       },
    { "(wilt)",         "Wilt"          },
    { ":P",             "Wink"          },
    { ":-O",            "Yawn"          },
};


static QHash<QString, QString> makeEmos()
{
    QHash<QString, QString> emos;
    for (size_t i = 0; i < sizeof emoticons / sizeof *emoticons; i++)
    {
        emoticon const &emo = emoticons[i];
        emos.insert(QString::fromUtf8(emo.gif), QString::fromUtf8(emo.emotion));
    }
    return emos;
}

static QHash<QString, QString> makeGifs()
{
    QHash<QString, QString> gifs;
    for (size_t i = 0; i < sizeof emoticons / sizeof *emoticons; i++)
    {
        emoticon const &emo = emoticons[i];
        gifs.insert(QString::fromUtf8(emo.emotion), QString::fromUtf8(emo.gif));
    }
    return gifs;
}


QHash<QString, QString> const Emoticons::emos = makeEmos();
QHash<QString, QString> const Emoticons::gifs = makeGifs();


Emoticons::Emoticons(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Emoticons)
{
    ui->setupUi(this);

    foreach (QObject *child, children())
        if (QPushButton *btn = qobject_cast<QPushButton *>(child))
            connect(btn, SIGNAL(clicked()), this, SLOT(iconClicked()));
}

Emoticons::~Emoticons()
{
    delete ui;
}

void Emoticons::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Emoticons::iconClicked()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    emit emote(emos.value(btn->objectName(), "?"));
}
