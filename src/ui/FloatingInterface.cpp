#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "../layout/AutoLayoutGenModule.hpp"
#include <algorithm>
#include <cmath>
#include <string>

using namespace geode::prelude;

namespace {
enum class Page { Home, Decoration, Layout, TriggerMarket, ObjectMarket, Tools };

bool insertGDObject(int objectID, int x, int y) {
    auto editor = LevelEditorLayer::get();
    if (!editor) return false;
    auto data = "1," + std::to_string(objectID) + ",2," + std::to_string(x) + ",3," + std::to_string(y);
    return editor->createObjectsFromString(gd::string(data.c_str()), false, false) != nullptr;
}

int applyGeneratedLayout(LayoutProfile const& profile) {
    auto editor = LevelEditorLayer::get();
    if (!editor) return -1;
    auto plan = AutoLayoutGenModule::generatePlan(profile);
    int x = 300;
    int inserted = 0;
    int groundY = 90;

    for (auto const& segment : plan) {
        int cells = std::clamp(static_cast<int>(std::round(segment.duration * 2.5f)), 5, 14);
        int obstacleCount = std::clamp(segment.jumpCount, 1, 4);
        int safeCells = segment.gapAfter ? std::max(3, cells - 2) : cells;

        // Keep a continuous playable floor, then shape each section with a distinct pattern.
        for (int i = 0; i < safeCells; ++i) {
            if (insertGDObject(1, x + i * 30, groundY)) ++inserted;
        }

        switch (segment.pattern % 5) {
            case 0: // classic obstacle rhythm
                for (int i = 0; i < obstacleCount; ++i) {
                    int spikeX = x + (2 + i * 2) * 30;
                    if (spikeX < x + safeCells * 30 && insertGDObject(8, spikeX, groundY + 30)) ++inserted;
                }
                break;
            case 1: // raised platform with a clear approach
                if (insertGDObject(2, x + 4 * 30, groundY + 60)) ++inserted;
                if (insertGDObject(2, x + 5 * 30, groundY + 60)) ++inserted;
                if (segment.difficultyWeight > 1.1f && insertGDObject(8, x + 7 * 30, groundY + 30)) ++inserted;
                break;
            case 2: // stair-step terrain
                for (int i = 0; i < std::min(obstacleCount, 3); ++i) {
                    if (insertGDObject(1, x + (3 + i) * 30, groundY + (i + 1) * 30)) ++inserted;
                }
                break;
            case 3: // low ceiling section
                for (int i = 0; i < std::min(cells - 2, 4); ++i) {
                    if (insertGDObject(1, x + (i + 2) * 30, groundY + 150)) ++inserted;
                }
                if (segment.difficultyWeight > 1.25f && insertGDObject(8, x + 5 * 30, groundY + 30)) ++inserted;
                break;
            default: // alternating blocks and spikes
                for (int i = 0; i < obstacleCount; ++i) {
                    int obstacleX = x + (3 + i * 2) * 30;
                    if (obstacleX >= x + safeCells * 30) break;
                    if (i % 2 == 0) {
                        if (insertGDObject(8, obstacleX, groundY + 30)) ++inserted;
                    } else {
                        if (insertGDObject(1, obstacleX, groundY + 60)) ++inserted;
                    }
                }
                break;
        }

        if (segment.isDrop) {
            // A visual drop marker is placed above the playable path.
            if (insertGDObject(36, x + safeCells * 15, groundY + 90)) ++inserted;
            if (insertGDObject(1, x + safeCells * 30, groundY + 30)) ++inserted;
        }

        x += (safeCells + (segment.gapAfter ? 2 : 1)) * 30;
    }

    editor->dirtifyTriggers();
    if (auto ui = EditorUI::get()) {
        ui->updateButtons();
        ui->updateObjectInfoLabel();
    }
    return inserted;
}

int applyDecorationPreset(std::string const& preset) {
    auto editor = LevelEditorLayer::get();
    if (!editor) return -1;
    auto objects = editor->getAllObjects();
    if (!objects || objects->count() == 0) return 0;
    float minX = 100000.f, maxX = -100000.f, minY = 100000.f, maxY = -100000.f;
    for (int i = 0; i < objects->count(); ++i) {
        auto object = static_cast<GameObject*>(objects->objectAtIndex(i));
        if (!object) continue;
        auto p = object->getPosition();
        minX = std::min(minX, p.x); maxX = std::max(maxX, p.x);
        minY = std::min(minY, p.y); maxY = std::max(maxY, p.y);
    }
    if (minX > maxX) return 0;
    int inserted = 0;
    int left = static_cast<int>(minX) - 60;
    int right = static_cast<int>(maxX) + 60;
    int top = static_cast<int>(maxY) + 90;
    int bottom = static_cast<int>(minY) - 90;
    int step = preset == "HELL" ? 60 : 90;
    for (int x = left; x <= right; x += step) {
        if (preset == "HELL") {
            if (insertGDObject(8, x, top)) ++inserted;
            if (insertGDObject(8, x, bottom)) ++inserted;
        } else if (preset == "TECHNO") {
            if (insertGDObject(1, x, top)) ++inserted;
            if (insertGDObject(1, x, bottom)) ++inserted;
            if ((x / step) % 2 == 0 && insertGDObject(1, x, top + 30)) ++inserted;
        } else if (preset == "MODERN") {
            if (insertGDObject(1, x, top)) ++inserted;
            if (insertGDObject(1, x, bottom)) ++inserted;
        }
    }
    editor->dirtifyTriggers();
    if (auto ui = EditorUI::get()) {
        ui->updateButtons();
        ui->updateObjectInfoLabel();
    }
    return inserted;
}

class DLPanel : public CCLayerColor {
    Page m_page = Page::Home;
    LayoutProfile m_profile;
    std::string m_status = "Escolha uma ferramenta.";
    std::string m_preset = "NULL";

    void addLabel(CCNode* parent, std::string const& text, CCPoint pos, float scale = .55f) {
        auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
        if (!label) return;
        label->setScale(scale);
        label->setPosition(pos);
        parent->addChild(label);
    }

    void rebuild() {
        this->removeAllChildrenWithCleanup(true);
        auto size = CCDirector::sharedDirector()->getWinSize();
        auto w = std::min(410.f, size.width - 28.f);
        auto h = std::min(270.f, size.height - 28.f);
        this->setContentSize(size);
        auto panel = CCLayerColor::create(ccc4(22, 20, 38, 248), w, h);
        panel->setPosition((size.width - w) / 2.f, (size.height - h) / 2.f);
        this->addChild(panel, 1);

        auto title = CCLabelBMFont::create("DL MEGA UTILS", "bigFont.fnt");
        title->setScale(.72f);
        title->setPosition(w / 2.f, h - 22.f);
        panel->addChild(title);

        auto closeLabel = CCLabelBMFont::create("X", "bigFont.fnt");
        closeLabel->setScale(.72f);
        auto close = CCMenuItemLabel::create(closeLabel, this, menu_selector(DLPanel::onAction));
        close->setTag(0);
        auto topMenu = CCMenu::create(close, nullptr);
        topMenu->setPosition(CCPointZero);
        close->setPosition(w - 19.f, h - 20.f);
        panel->addChild(topMenu, 5);

        auto makeButton = [&](std::string const& text, int tag, float y) {
            auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
            if (!label) return;
            label->setScale(.48f);
            auto item = CCMenuItemLabel::create(label, this, menu_selector(DLPanel::onAction));
            item->setTag(tag);
            auto menu = CCMenu::create(item, nullptr);
            menu->setPosition(CCPointZero);
            panel->addChild(menu, 4);
            item->setPosition(w / 2.f, y);
        };

        if (m_page == Page::Home) {
            makeButton("AUTO DECORATION", 1, h - 66.f);
            makeButton("AUTO LAYOUT GEN", 2, h - 101.f);
            makeButton("TRIGGER MARKET", 3, h - 136.f);
            makeButton("OBJECT MARKET", 4, h - 171.f);
            makeButton("EDITOR TOOLS", 5, h - 206.f);
        } else {
            makeButton("< VOLTAR", 10, 24.f);
            if (m_page == Page::Decoration) {
                addLabel(panel, "PRESET: " + m_preset, { w / 2.f, h - 61.f });
                makeButton("NULL", 16, h - 100.f);
                makeButton("HELL", 17, h - 132.f);
                makeButton("TECHNO", 18, h - 164.f);
                makeButton("MODERN", 19, h - 196.f);
                makeButton("APPLY PRESET", 20, h - 228.f);
                addLabel(panel, m_status, { w / 2.f, 48.f }, .34f);
            } else if (m_page == Page::Layout) {
                auto diff = static_cast<int>(m_profile.difficulty);
                auto len = static_cast<int>(m_profile.length);
                auto sync = static_cast<int>(m_profile.sync);
                const char* diffs[] = { "EASY", "NORMAL", "HARD", "INSANE", "DEMON" };
                const char* lengths[] = { "SHORT", "MEDIUM", "LONG" };
                const char* syncs[] = { "LOW", "NORMAL", "HIGH" };
                makeButton(std::string("DIFFICULTY: ") + diffs[diff], 11, h - 65.f);
                makeButton(std::string("LENGTH: ") + lengths[len], 12, h - 98.f);
                makeButton(std::string("SYNC: ") + syncs[sync], 13, h - 131.f);
                makeButton(std::string("DROPS: ") + (m_profile.includeDrops ? "ON" : "OFF"), 14, h - 164.f);
                makeButton("GENERATE PLAN", 15, h - 197.f);
                addLabel(panel, m_status, { w / 2.f, 47.f }, .32f);
            } else if (m_page == Page::TriggerMarket) {
                addLabel(panel, "TRIGGER MARKET", { w / 2.f, h - 55.f }, .62f);
                makeButton("[MV] MOVE", 30, h - 91.f);
                makeButton("[CL] COLOR", 31, h - 119.f);
                makeButton("[PL] PULSE", 32, h - 147.f);
                makeButton("[AL] ALPHA", 33, h - 175.f);
                makeButton("[TG] TOGGLE", 34, h - 203.f);
                makeButton("[SP] SPAWN", 35, h - 231.f);
                addLabel(panel, m_status, { w / 2.f, 43.f }, .30f);
            } else if (m_page == Page::ObjectMarket) {
                addLabel(panel, "OBJECT MARKET", { w / 2.f, h - 70.f });
                addLabel(panel, "Biblioteca online ainda nao conectada.", { w / 2.f, h - 115.f }, .36f);
                addLabel(panel, "Nenhum objeto sera baixado ficticiamente.", { w / 2.f, h - 145.f }, .34f);
            } else if (m_page == Page::Tools) {
                addLabel(panel, "EDITOR TOOLS", { w / 2.f, h - 70.f });
                addLabel(panel, "Ferramentas serao adicionadas aqui.", { w / 2.f, h - 115.f }, .38f);
                addLabel(panel, "Esta tela e apenas o ponto de entrada.", { w / 2.f, h - 145.f }, .34f);
            }
        }
    }

    void open(Page page) {
        m_page = page;
        rebuild();
    }

    void onAction(CCObject* sender) {
        auto tag = sender->getTag();
        if (tag == 0) { this->removeFromParentAndCleanup(true); return; }
        if (tag == 10) { open(Page::Home); return; }
        if (tag == 1) { open(Page::Decoration); return; }
        if (tag == 2) { open(Page::Layout); return; }
        if (tag == 3) { open(Page::TriggerMarket); return; }
        if (tag == 4) { open(Page::ObjectMarket); return; }
        if (tag == 5) { open(Page::Tools); return; }
        if (tag >= 16 && tag <= 19) {
            const char* presets[] = { "NULL", "HELL", "TECHNO", "MODERN" };
            m_preset = presets[tag - 16];
            m_status = "Preset " + m_preset + " selecionado.";
            rebuild();
            return;
        }
        if (tag == 20) {
            auto inserted = applyDecorationPreset(m_preset);
            if (inserted < 0) m_status = "Abra uma fase no editor primeiro.";
            else if (inserted == 0) m_status = "Nenhum objeto encontrado para decorar.";
            else m_status = "Decoracao aplicada: " + std::to_string(inserted) + " objetos.";
        } else if (tag >= 30 && tag <= 35) {
            static const int triggerIDs[] = { 901, 899, 1006, 1007, 1049, 1268 };
            static const char* triggerNames[] = { "MOVE", "COLOR", "PULSE", "ALPHA", "TOGGLE", "SPAWN" };
            auto editor = LevelEditorLayer::get();
            if (!editor) {
                m_status = "Abra uma fase no editor primeiro.";
            } else {
                static int nextTriggerX = 300;
                bool added = insertGDObject(triggerIDs[tag - 30], nextTriggerX, 150);
                if (added) {
                    nextTriggerX += 45;
                    editor->dirtifyTriggers();
                    if (auto ui = EditorUI::get()) {
                        ui->updateButtons();
                        ui->updateObjectInfoLabel();
                    }
                    m_status = std::string(triggerNames[tag - 30]) + " adicionado! Configure no editor.";
                } else {
                    m_status = "Nao foi possivel inserir esse trigger.";
                }
            }
        } else if (tag == 11) {
            m_profile.difficulty = static_cast<LayoutDifficulty>((static_cast<int>(m_profile.difficulty) + 1) % 5);
        } else if (tag == 12) {
            m_profile.length = static_cast<LayoutLength>((static_cast<int>(m_profile.length) + 1) % 3);
        } else if (tag == 13) {
            m_profile.sync = static_cast<SyncIntensity>((static_cast<int>(m_profile.sync) + 1) % 3);
        } else if (tag == 14) {
            m_profile.includeDrops = !m_profile.includeDrops;
        } else if (tag == 15) {
            auto inserted = applyGeneratedLayout(m_profile);
            if (inserted < 0) m_status = "Abra uma fase no editor primeiro.";
            else if (inserted == 0) m_status = "Nenhum objeto foi criado.";
            else m_status = "Layout aplicado: " + std::to_string(inserted) + " objetos.";
        }
        rebuild();
    }

public:
    static DLPanel* create() {
        auto ret = new DLPanel();
        auto size = CCDirector::sharedDirector()->getWinSize();
        if (ret && ret->initWithColor(ccc4(0, 0, 0, 150), size.width, size.height)) {
            ret->autorelease();
            ret->rebuild();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class DLFloatingOrb : public CCLayer {
    CCSprite* m_orb = nullptr;
    bool m_dragging = false;
    bool m_moved = false;
    CCPoint m_touchStart;
    CCPoint m_orbStart;
    void togglePanel() {
        auto existing = this->getChildByID("dl-panel");
        if (existing) {
            existing->removeFromParentAndCleanup(true);
            return;
        }
        auto panel = DLPanel::create();
        if (panel) {
            panel->setID("dl-panel");
            this->addChild(panel, 20);
        }
    }

public:
    static DLFloatingOrb* create() {
        auto ret = new DLFloatingOrb();
        if (ret && ret->init()) { ret->autorelease(); return ret; }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init() override {
        if (!CCLayer::init()) return false;
        m_orb = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        if (!m_orb) m_orb = CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png");
        if (!m_orb) return true;
        m_orb->setScale(1.25f);
        auto win = CCDirector::sharedDirector()->getWinSize();
        m_orb->setPosition(CCPoint(38.f, win.height * .55f));
        this->addChild(m_orb, 2);
        auto orbText = CCLabelBMFont::create("DL", "bigFont.fnt");
        if (orbText) { orbText->setScale(.30f); orbText->setPosition(m_orb->getContentSize().width / 2.f, m_orb->getContentSize().height / 2.f); m_orb->addChild(orbText, 5); }
        return true;
    }

    void onEnter() override {
        CCLayer::onEnter();
        CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -500, true);
    }

    void onExit() override {
        CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
        CCLayer::onExit();
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent*) override {
        if (this->getChildByID("dl-panel") || !m_orb) return false;
        auto p = this->convertToNodeSpace(touch->getLocation());
        if (!m_orb->boundingBox().containsPoint(p)) return false;
        m_dragging = true;
        m_moved = false;
        m_touchStart = p;
        m_orbStart = m_orb->getPosition();
        return true;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent*) override {
        if (!m_dragging || !m_orb) return;
        auto p = this->convertToNodeSpace(touch->getLocation());
        auto delta = p - m_touchStart;
        if (std::abs(delta.x) + std::abs(delta.y) > 7.f) m_moved = true;
        auto win = CCDirector::sharedDirector()->getWinSize();
        auto x = std::clamp(m_orbStart.x + delta.x, 24.f, win.width - 24.f);
        auto y = std::clamp(m_orbStart.y + delta.y, 24.f, win.height - 24.f);
        m_orb->setPosition(CCPoint(x, y));
    }

    void ccTouchEnded(CCTouch*, CCEvent*) override {
        if (!m_dragging) return;
        m_dragging = false;
        if (!m_moved) togglePanel();
    }
};

void addFloatingOrb(CCNode* parent) {
    if (!parent || parent->getChildByID("dl-floating-orb")) return;
    auto orb = DLFloatingOrb::create();
    if (!orb) return;
    orb->setID("dl-floating-orb");
    parent->addChild(orb, 999);
}
}

class $modify(DLMegaMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        addFloatingOrb(this);
        return true;
    }
};

class $modify(DLMegaEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editor) {
        if (!EditorUI::init(editor)) return false;
        addFloatingOrb(this);
        return true;
    }
};
