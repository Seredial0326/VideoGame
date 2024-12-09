#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

extern int playerHealth;
extern int rivalHealth;
extern int playerHealth = 100;
extern int rivalHealth = 100;
extern int MAX_HEALTH = 100;
extern int playerFatigue = 100;
extern const int fatigueThreshold = 30; // Tiempo mínimo para poder atacar
extern int rivalFatigue = 100; // Fatiga del rival
extern const int rivalFatigueThreshold = 20; // Tiempo mínimo para atacar
extern bool isReacting = false;  // Indica si está en tiempo de reacción
extern std::string rivalAttackType = ""; // Tipo de ataque del rival
extern float reactionTime = 1.5f; // Tiempo en segundos para reaccionar

extern int totalTime = 180;
extern int playerReaction = 0;

extern int attackType = 0;
extern int reactionAct = 0;
extern bool lado = false;

// Sprites para las barras de salud
cocos2d::Sprite* playerHealthBar;
cocos2d::Sprite* rivalHealthBar;
cocos2d::Sprite* playerFatigueBar;

#endif // GLOBAL_VARIABLES_H
