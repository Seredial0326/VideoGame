#include "ResourceManager.h"

using namespace cocos2d;

ResourceManager* ResourceManager::getInstance() {
    static ResourceManager instance;
    return &instance;
}

void ResourceManager::loadSpriteFrames() {

    spriteFrames["1"] = SpriteFrame::create("1.png", Rect(0, 0, 156, 380));
    spriteFrames["2"] = SpriteFrame::create("2.png", Rect(0, 0, 340, 380));
    spriteFrames["3"] = SpriteFrame::create("3.png", Rect(0, 0, 328, 384));
    spriteFrames["4"] = SpriteFrame::create("4.png", Rect(0, 0, 316, 384));
    spriteFrames["5"] = SpriteFrame::create("5.png", Rect(0, 0, 324, 384));
    spriteFrames["6"] = SpriteFrame::create("6.png", Rect(0, 0, 272, 384));
    spriteFrames["7"] = SpriteFrame::create("7.png", Rect(0, 0, 332, 384));
    spriteFrames["8"] = SpriteFrame::create("8.png", Rect(0, 0, 308, 384));
    spriteFrames["9"] = SpriteFrame::create("9.png", Rect(0, 0, 276, 384));
    spriteFrames["10"] = SpriteFrame::create("10.png", Rect(0, 0, 460, 384));

    spriteFrames["Titulo"] = SpriteFrame::create("Gametitle.png", Rect(0, 0, 962, 352));

    spriteFrames["LM_PI"] = SpriteFrame::create("LM5_resize.png", Rect(0, 0, 325, 500));
    spriteFrames["LM_JabIzq1"] = SpriteFrame::create("LM18.png", Rect(0, 0, 377, 500));
    spriteFrames["LM_JabIzq2"] = SpriteFrame::create("LM19.png", Rect(0, 0, 300, 621));
    spriteFrames["LM_JabDer1"] = SpriteFrame::create("LM18_D.png", Rect(0, 0, 377, 500));
    spriteFrames["LM_JabDer2"] = SpriteFrame::create("LM19_D.png", Rect(0, 0, 300, 621));

    spriteFrames["LM_GanIzq1"] = SpriteFrame::create("LM31.png", Rect(0, 0, 521, 495));
    spriteFrames["LM_GanIzq2"] = SpriteFrame::create("LM3.png", Rect(0, 0, 563, 736));
    spriteFrames["LM_GanIzq3"] = SpriteFrame::create("LM4.png", Rect(0, 0, 453, 700));
    spriteFrames["LM_GanIzq4"] = SpriteFrame::create("LM21.png", Rect(0, 0, 412, 510));
    spriteFrames["LM_GanDer1"] = SpriteFrame::create("LM31_D.png", Rect(0, 0, 521, 495));
    spriteFrames["LM_GanDer2"] = SpriteFrame::create("LM3_D.png", Rect(0, 0, 563, 736));
    spriteFrames["LM_GanDer3"] = SpriteFrame::create("LM4_D.png", Rect(0, 0, 453, 700));
    spriteFrames["LM_GanDer4"] = SpriteFrame::create("LM21_D.png", Rect(0, 0, 412, 510));

    spriteFrames["LM_GBI2"] = SpriteFrame::create("LM12.png", Rect(0, 0, 350, 504));
    spriteFrames["LM_GBI1"] = SpriteFrame::create("LM11.png", Rect(0, 0, 407, 505));
    spriteFrames["LM_GBD2"] = SpriteFrame::create("LM12_D.png", Rect(0, 0, 350, 504));
    spriteFrames["LM_GBD1"] = SpriteFrame::create("LM11_D.png", Rect(0, 0, 407, 505));

    spriteFrames["LM_GR"] = SpriteFrame::create("LM10.png", Rect(0, 0, 278, 479));
    spriteFrames["LM_ES"] = SpriteFrame::create("LM6.png", Rect(0, 0, 288, 494));
    spriteFrames["LM_KOI2"] = SpriteFrame::create("LM8.png", Rect(0, 0, 551, 371));
    
    spriteFrames["LM_EI"] = SpriteFrame::create("LM17.png", Rect(0, 0, 464, 634));
    spriteFrames["LM_ED"] = SpriteFrame::create("LM17_D.png", Rect(0, 0, 464, 634));

    spriteFrames["LM_DD"] = SpriteFrame::create("LM15.png", Rect(0, 0, 423, 506));
    spriteFrames["LM_DI"] = SpriteFrame::create("LM15_I.png", Rect(0, 0, 423, 506));

    spriteFrames["LM_KOD1"] = SpriteFrame::create("LM28.png", Rect(0, 0, 80, 110));
    spriteFrames["LM_KOI1"] = SpriteFrame::create("LM28_I.png", Rect(0, 0, 80, 110));

    spriteFrames["LM_S1"] = SpriteFrame::create("LM29.png", Rect(0, 0, 490, 660));
    spriteFrames["LM_S2"] = SpriteFrame::create("LM1.png", Rect(0, 0, 484, 700));
    spriteFrames["LM_S3"] = SpriteFrame::create("LM30.png", Rect(0, 0, 423, 702));
    spriteFrames["LM_S4"] = SpriteFrame::create("LM2.png", Rect(0, 0, 407, 701));
    spriteFrames["LM_S5"] = SpriteFrame::create("LM20.png", Rect(0, 0, 407, 541));

    spriteFrames["LM_VIC1"] = SpriteFrame::create("LM22.png", Rect(0, 0, 407, 701));
    spriteFrames["LM_VIC2"] = SpriteFrame::create("LM23.png", Rect(0, 0, 407, 541));

    spriteFrames["BH_juego"] = SpriteFrame::create("BH_juego.png", Rect(0, 0, 510, 803));
    spriteFrames["BH_juego2"] = SpriteFrame::create("BH_juego2.png", Rect(0, 0, 510, 803));
    spriteFrames["BH_Prep1"] = SpriteFrame::create("BH3.png", Rect(0, 0, 510, 803));
    spriteFrames["BH_Prep2"] = SpriteFrame::create("BH_4.png", Rect(0, 0, 510, 803));
    spriteFrames["BH_Prov"] = SpriteFrame::create("BH6.png", Rect(0, 0, 535, 795));

    spriteFrames["BH_PI"] = SpriteFrame::create("BH_juego3.png", Rect(0, 0, 510, 803)); //101,
    spriteFrames["BH_Guardia"] = SpriteFrame::create("BH9.png", Rect(0, 0, 510, 803));
    spriteFrames["BH_Risa"] = SpriteFrame::create("BH10.png", Rect(0, 0, 472, 795));
    spriteFrames["BH_Lengua"] = SpriteFrame::create("BH11.png", Rect(0, 0, 510, 800));

    spriteFrames["BH_JabIzq1"] = SpriteFrame::create("BH7.png", Rect(0, 0, 510, 803));
    spriteFrames["BH_JabIzq2"] = SpriteFrame::create("BH8.png", Rect(0, 0, 510, 808));
    spriteFrames["BH_GAI1"] = SpriteFrame::create("BH12.png", Rect(0, 0, 469, 810));
    spriteFrames["BH_GAI2"] = SpriteFrame::create("BH13.png", Rect(0, 0, 444, 795));
    spriteFrames["BH_GAI3"] = SpriteFrame::create("BH14.png", Rect(0, 0, 515, 795));

    spriteFrames["BH_GD1"] = SpriteFrame::create("BH15.png", Rect(0, 0, 780, 900));
    spriteFrames["BH_GD2"] = SpriteFrame::create("BH16.png", Rect(0, 0, 510, 803));
    spriteFrames["BH_GDG"] = SpriteFrame::create("BH_16.png", Rect(0, 0, 650, 800));

    spriteFrames["BH_EI1"] = SpriteFrame::create("BH_17.png", Rect(0, 0, 510, 833));
    spriteFrames["BH_EI2"] = SpriteFrame::create("BH18.png", Rect(0, 0, 621, 843));

    spriteFrames["BH_ED1"] = SpriteFrame::create("BH17_D.png", Rect(0, 0, 510, 833));
    spriteFrames["BH_ED2"] = SpriteFrame::create("BH18_D.png", Rect(0, 0, 621, 843));

    spriteFrames["BH_GPI1"] = SpriteFrame::create("BH19.png", Rect(0, 0, 510, 813));
    spriteFrames["BH_GPD1"] = SpriteFrame::create("BH19_D.png", Rect(0, 0, 510, 813));
    spriteFrames["BH_GPU"] = SpriteFrame::create("BH21.png", Rect(0, 0, 510, 879));

    spriteFrames["BH_GPAI"] = SpriteFrame::create("BH22.png", Rect(0, 0, 615, 875));
    spriteFrames["BH_GPAD"] = SpriteFrame::create("BH22_D.png", Rect(0, 0, 615, 875));
    spriteFrames["BH_Disos"] = SpriteFrame::create("BH20.png", Rect(0, 0, 525, 805));
    spriteFrames["BH_KOI1"] = SpriteFrame::create("BH23.png", Rect(0, 0, 510, 910));
    spriteFrames["BH_KOI2"] = SpriteFrame::create("BH24.png", Rect(0, 0, 510, 910));

    spriteFrames["BH_KOIT"] = SpriteFrame::create("BH27.png", Rect(0, 0, 605, 795));
    spriteFrames["BH_KOTD"] = SpriteFrame::create("BH26.png", Rect(0, 0, 605, 795));
    spriteFrames["BH_levantado"] = SpriteFrame::create("BH28.png", Rect(0, 0, 605, 795));
    spriteFrames["BH_levantado2"] = SpriteFrame::create("BH29.png", Rect(0, 0, 605, 795));

    spriteFrames["NK1"] = SpriteFrame::create("NK1.png", Rect(0, 0, 276, 704));
    spriteFrames["NK2"] = SpriteFrame::create("NK2.png", Rect(0, 0, 308, 720));
    spriteFrames["NK3"] = SpriteFrame::create("NK3.png", Rect(0, 0, 352, 696));
    spriteFrames["NK4"] = SpriteFrame::create("NK4.png", Rect(0, 0, 308, 676));
    spriteFrames["NK5"] = SpriteFrame::create("NK5.png", Rect(0, 0, 320, 692));

    spriteFrames["NK6"] = SpriteFrame::create("NK6.png", Rect(0, 0, 320, 692));
    spriteFrames["NK7"] = SpriteFrame::create("NK7.png", Rect(0, 0, 308, 684));
    spriteFrames["NK8"] = SpriteFrame::create("NK8.png", Rect(0, 0, 308, 684));
    spriteFrames["NK9"] = SpriteFrame::create("NK9.png", Rect(0, 0, 288, 672));

    spriteFrames["NK10"] = SpriteFrame::create("NK10.png", Rect(0, 0, 296, 684));
    spriteFrames["NK11"] = SpriteFrame::create("NK11.png", Rect(0, 0, 336, 676));
    spriteFrames["NK12"] = SpriteFrame::create("NK12.png", Rect(0, 0, 352, 680));
    spriteFrames["NK13"] = SpriteFrame::create("NK13.png", Rect(0, 0, 316, 672));
    spriteFrames["NK14"] = SpriteFrame::create("NK14.png", Rect(0, 0, 360, 624));

    spriteFrames["NK15"] = SpriteFrame::create("NK15.png", Rect(0, 0, 308, 640));
    spriteFrames["NK16"] = SpriteFrame::create("NK16.png", Rect(0, 0, 388, 664));
    spriteFrames["NK17"] = SpriteFrame::create("NK17.png", Rect(0, 0, 344, 596));

    spriteFrames["NK18"] = SpriteFrame::create("NK18.png", Rect(0, 0, 316, 640));
    spriteFrames["NK19"] = SpriteFrame::create("NK19.png", Rect(0, 0, 308, 740));

    spriteFrames["NK20"] = SpriteFrame::create("NK20.png", Rect(0, 0, 352, 560));
    spriteFrames["NK21"] = SpriteFrame::create("NK21.png", Rect(0, 0, 344, 648));

    spriteFrames["NK22"] = SpriteFrame::create("NK22.png", Rect(0, 0, 384, 532));
    spriteFrames["NK23"] = SpriteFrame::create("NK23.png", Rect(0, 0, 300, 636));
    spriteFrames["NK24"] = SpriteFrame::create("NK24.png", Rect(0, 0, 320, 672));

    spriteFrames["NK25"] = SpriteFrame::create("NK25.png", Rect(0, 0, 332, 672));
    spriteFrames["NK26"] = SpriteFrame::create("NK26.png", Rect(0, 0, 332, 680));
    spriteFrames["NK27"] = SpriteFrame::create("NK27.png", Rect(0, 0, 396, 680));
    spriteFrames["NK28"] = SpriteFrame::create("NK28.png", Rect(0, 0, 296, 692));
    spriteFrames["NK29"] = SpriteFrame::create("NK29.png", Rect(0, 0, 416, 664));

    spriteFrames["NK30"] = SpriteFrame::create("NK30.png", Rect(0, 0, 396, 684));
    spriteFrames["NK31"] = SpriteFrame::create("NK31.png", Rect(0, 0, 364, 640));
    spriteFrames["NK32"] = SpriteFrame::create("NK32.png", Rect(0, 0, 364, 644));
    spriteFrames["NK33"] = SpriteFrame::create("NK33.png", Rect(0, 0, 384, 644)); 

    spriteFrames["NK34"] = SpriteFrame::create("NK34.png", Rect(0, 0, 320, 612));
    spriteFrames["NK35"] = SpriteFrame::create("NK35.png", Rect(0, 0, 508, 428));
    spriteFrames["NK36"] = SpriteFrame::create("NK36.png", Rect(0, 0, 408, 340));
    spriteFrames["NK37"] = SpriteFrame::create("NK37.png", Rect(0, 0, 368, 568));
    spriteFrames["NK38"] = SpriteFrame::create("NK38.png", Rect(0, 0, 364, 676));

    spriteFrames["NK39"] = SpriteFrame::create("NK39.png", Rect(0, 0, 364, 680));
    spriteFrames["NK40"] = SpriteFrame::create("NK40.png", Rect(0, 0, 344, 688));
    spriteFrames["NK41"] = SpriteFrame::create("NK41.png", Rect(0, 0, 268, 668));
    spriteFrames["NK42"] = SpriteFrame::create("NK42.png", Rect(0, 0, 276, 720));
    spriteFrames["NK43"] = SpriteFrame::create("NK43.png", Rect(0, 0, 276, 716));


    spriteFrames["NK44"] = SpriteFrame::create("NK44.png", Rect(0, 0, 276, 716));
    spriteFrames["NK45"] = SpriteFrame::create("NK45.png", Rect(0, 0, 300, 708));
    spriteFrames["NK46"] = SpriteFrame::create("NK46.png", Rect(0, 0, 300, 696));
    spriteFrames["NK47"] = SpriteFrame::create("NK47.png", Rect(0, 0, 288, 672));
    spriteFrames["NK48"] = SpriteFrame::create("NK48.png", Rect(0, 0, 336, 676));

    spriteFrames["NK49"] = SpriteFrame::create("NK49.png", Rect(0, 0, 352, 680));
    spriteFrames["NK50"] = SpriteFrame::create("NK50.png", Rect(0, 0, 296, 684));
    spriteFrames["NK51"] = SpriteFrame::create("NK51.png", Rect(0, 0, 316, 672));
    spriteFrames["NK52"] = SpriteFrame::create("NK52.png", Rect(0, 0, 352, 560));
    spriteFrames["NK53"] = SpriteFrame::create("NK53.png", Rect(0, 0, 344, 596));

    spriteFrames["NK54"] = SpriteFrame::create("NK54.png", Rect(0, 0, 308, 740));
    spriteFrames["NK55"] = SpriteFrame::create("NK55.png", Rect(0, 0, 316, 640));
    spriteFrames["NK56"] = SpriteFrame::create("NK56.png", Rect(0, 0, 320, 672));
    spriteFrames["NK57"] = SpriteFrame::create("NK57.png", Rect(0, 0, 332, 672));

    spriteFrames["NK58"] = SpriteFrame::create("NK58.png", Rect(0, 0, 332, 680));
    spriteFrames["NK59"] = SpriteFrame::create("NK59.png", Rect(0, 0, 396, 680));
    spriteFrames["NK60"] = SpriteFrame::create("NK60.png", Rect(0, 0, 416, 664));
    spriteFrames["NK61"] = SpriteFrame::create("NK61.png", Rect(0, 0, 396, 684));

    spriteFrames["NK62"] = SpriteFrame::create("NK62.png", Rect(0, 0, 364, 640));
    spriteFrames["NK63"] = SpriteFrame::create("NK63.png", Rect(0, 0, 360, 624));
    spriteFrames["NK64"] = SpriteFrame::create("NK64.png", Rect(0, 0, 308, 640));
    spriteFrames["NK65"] = SpriteFrame::create("NK65.png", Rect(0, 0, 344, 596));

    spriteFrames["GJ1"] = SpriteFrame::create("GJ1.png", Rect(0, 0, 228, 656));
    spriteFrames["GJ2"] = SpriteFrame::create("GJ2.png", Rect(0, 0, 196, 656));
    spriteFrames["GJ3"] = SpriteFrame::create("GJ3.png", Rect(0, 0, 235, 650));
    spriteFrames["GJ4"] = SpriteFrame::create("GJ4.png", Rect(0, 0, 200, 652));
    spriteFrames["GJ5"] = SpriteFrame::create("GJ5.png", Rect(0, 0, 208, 600));

    spriteFrames["GJ6"] = SpriteFrame::create("GJ6.png", Rect(0, 0, 280, 636)); //101,
    spriteFrames["GJ7"] = SpriteFrame::create("GJ7.png", Rect(0, 0, 240, 640));
    spriteFrames["GJ8"] = SpriteFrame::create("GJ8.png", Rect(0, 0, 180, 580));
    spriteFrames["GJ9"] = SpriteFrame::create("GJ9.png", Rect(0, 0, 264, 640));

    spriteFrames["GJ10"] = SpriteFrame::create("GJ10.png", Rect(0, 0, 220, 636));
    spriteFrames["GJ11"] = SpriteFrame::create("GJ11.png", Rect(0, 0, 276, 612));
    spriteFrames["GJ12"] = SpriteFrame::create("GJ12.png", Rect(0, 0, 244, 616));
    spriteFrames["GJ13"] = SpriteFrame::create("GJ13.png", Rect(0, 0, 288, 636));
    spriteFrames["GJ14"] = SpriteFrame::create("GJ14.png", Rect(0, 0, 220, 540));

    spriteFrames["GJ15"] = SpriteFrame::create("GJ15.png", Rect(0, 0, 240, 584));
    spriteFrames["GJ16"] = SpriteFrame::create("GJ16.png", Rect(0, 0, 280, 576));
    spriteFrames["GJ17"] = SpriteFrame::create("GJ17.png", Rect(0, 0, 220, 540));

    spriteFrames["GJ18"] = SpriteFrame::create("GJ18.png", Rect(0, 0, 276, 612));
    spriteFrames["GJ19"] = SpriteFrame::create("GJ19.png", Rect(0, 0, 332, 612));

    spriteFrames["GJ20"] = SpriteFrame::create("GJ20.png", Rect(0, 0, 276, 612));
    spriteFrames["GJ21"] = SpriteFrame::create("GJ21.png", Rect(0, 0, 240, 584));

    spriteFrames["GJ22"] = SpriteFrame::create("GJ22.png", Rect(0, 0, 280, 576));
    spriteFrames["GJ23"] = SpriteFrame::create("GJ23.png", Rect(0, 0, 256, 668));
    spriteFrames["GJ24"] = SpriteFrame::create("GJ24.png", Rect(0, 0, 264, 640));

    spriteFrames["GJ25"] = SpriteFrame::create("GJ25.png", Rect(0, 0, 236, 600));
    spriteFrames["GJ26"] = SpriteFrame::create("GJ26.png", Rect(0, 0, 236, 584));
    spriteFrames["GJ27"] = SpriteFrame::create("GJ27.png", Rect(0, 0, 208, 600));
    spriteFrames["GJ28"] = SpriteFrame::create("GJ28.png", Rect(0, 0, 256, 668));
    spriteFrames["GJ29"] = SpriteFrame::create("GJ29.png", Rect(0, 0, 208, 600));

    spriteFrames["GJ30"] = SpriteFrame::create("GJ30.png", Rect(0, 0, 264, 640));
    spriteFrames["GJ31"] = SpriteFrame::create("GJ31.png", Rect(0, 0, 236, 652));
    spriteFrames["GJ32"] = SpriteFrame::create("GJ32.png", Rect(0, 0, 200, 652));
    spriteFrames["GJ33"] = SpriteFrame::create("GJ33.png", Rect(0, 0, 216, 624));

    spriteFrames["GJ34"] = SpriteFrame::create("GJ34.png", Rect(0, 0, 280, 520));
    spriteFrames["GJ35"] = SpriteFrame::create("GJ35.png", Rect(0, 0, 232, 628));
    spriteFrames["GJ36"] = SpriteFrame::create("GJ36.png", Rect(0, 0, 324, 612));
    spriteFrames["GJ37"] = SpriteFrame::create("GJ37.png", Rect(0, 0, 240, 632));
    spriteFrames["GJ38"] = SpriteFrame::create("GJ38.png", Rect(0, 0, 324, 512));

    spriteFrames["GJ39"] = SpriteFrame::create("GJ39.png", Rect(0, 0, 392, 332));
    spriteFrames["GJ40"] = SpriteFrame::create("GJ40.png", Rect(0, 0, 364, 348));
    spriteFrames["GJ41"] = SpriteFrame::create("GJ41.png", Rect(0, 0, 336, 504));
    spriteFrames["GJ42"] = SpriteFrame::create("GJ42.png", Rect(0, 0, 232, 640));
    spriteFrames["GJ43"] = SpriteFrame::create("GJ43.png", Rect(0, 0, 216, 640));


    spriteFrames["GJ44"] = SpriteFrame::create("GJ44.png", Rect(0, 0, 280, 636));
    spriteFrames["GJ45"] = SpriteFrame::create("GJ45.png", Rect(0, 0, 364, 640));
    spriteFrames["GJ46"] = SpriteFrame::create("GJ46.png", Rect(0, 0, 220, 636));
    spriteFrames["GJ47"] = SpriteFrame::create("GJ47.png", Rect(0, 0, 276, 612));
    spriteFrames["GJ48"] = SpriteFrame::create("GJ48.png", Rect(0, 0, 244, 616));

    spriteFrames["GJ49"] = SpriteFrame::create("GJ49.png", Rect(0, 0, 220, 540));
    spriteFrames["GJ50"] = SpriteFrame::create("GJ50.png", Rect(0, 0, 288, 636));
    spriteFrames["GJ51"] = SpriteFrame::create("GJ51.png", Rect(0, 0, 240, 584));
    spriteFrames["GJ52"] = SpriteFrame::create("GJ52.png", Rect(0, 0, 280, 576));
    spriteFrames["GJ53"] = SpriteFrame::create("GJ53.png", Rect(0, 0, 220, 540));

    spriteFrames["GJ54"] = SpriteFrame::create("GJ54.png", Rect(0, 0, 216, 624));
    spriteFrames["GJ55"] = SpriteFrame::create("GJ55.png", Rect(0, 0, 280, 520));
    spriteFrames["GJ56"] = SpriteFrame::create("GJ56.png", Rect(0, 0, 232, 628));
    spriteFrames["GJ57"] = SpriteFrame::create("GJ57.png", Rect(0, 0, 324, 612));
}

SpriteFrame* ResourceManager::getSpriteFrame(const std::string& name) {
    if (spriteFrames.find(name) != spriteFrames.end()) {
        return spriteFrames[name];
    }
    log("SpriteFrame %s not found!", name.c_str());
    return nullptr;
}
