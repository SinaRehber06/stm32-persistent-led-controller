#include "main.h"

/* Değişken Tanımlamaları */
TIM_HandleTypeDef htim2;
uint32_t blink_count = 4;      
uint8_t toggle_state = 0;      
uint8_t current_blinks = 0;    
uint8_t wait_half_seconds = 0; 
uint8_t is_waiting = 0;        

/* Fonksiyon Prototipleri */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
void Flash_Write(uint32_t data);

/* Flash Yazma Fonksiyonu (Hafızaya Kayıt) */
void Flash_Write(uint32_t data) {
    HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = 0x0800FC00;
    EraseInitStruct.NbPages = 1;
    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x0800FC00, data);
    HAL_FLASH_Lock();
}

/* Timer Kesme Fonksiyonu (Blink ve 5sn Bekleme) */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        if (is_waiting == 1) {
            wait_half_seconds++;
            if (wait_half_seconds >= 10) {
                is_waiting = 0;       
                wait_half_seconds = 0;     
                current_blinks = 0;
                toggle_state = 0; 
            }
        } else {
            if (toggle_state == 0) {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
                toggle_state = 1;
            } else {
                HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
                toggle_state = 0;
                current_blinks++; 
                if (current_blinks >= blink_count) {
                    is_waiting = 1; 
                }
            }
        }
    }
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM2_Init();

    /* --- g MADDESİ: GÜÇ VERİLİRKEN RESETLEME (EL TİTREMESİ AFFEDEN VERSİYON) --- */
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
        uint32_t startup_tick = HAL_GetTick();
        uint32_t last_high = startup_tick;
        uint8_t reset_success = 0;

        // 3 saniye boyunca elin titrese bile sabret
        while (1) {
            if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
                last_high = HAL_GetTick(); // Temas oldukça güncelle
            }

            // Toplam 3 saniye doldu mu?
            if ((HAL_GetTick() - startup_tick) >= 3000) {
                reset_success = 1;
                break;
            }

            // Kablo tam yarım saniye (500ms) boyunca metaldan tamamen uzak kalırsa iptal et
            if ((HAL_GetTick() - last_high) > 500) {
                break;
            }
        }

        // 3 saniye dolduysa sıfırla!
        if (reset_success == 1) {
            blink_count = 4;           
            Flash_Write(blink_count);  
            
            // İŞLEM BAŞARILI SİNYALİ: LED 1 saniye sabit yanar!
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
            HAL_Delay(1000);
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
        }
    }

    /* Flash Bellekten Oku */
    blink_count = *(uint32_t*)0x0800FC00;
    if (blink_count > 7 || blink_count < 4) {
        blink_count = 4;           
        Flash_Write(blink_count);  
    }

    /* Başlangıç Ayarları ve Timer Başlatma */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); 
    HAL_TIM_Base_Start_IT(&htim2);

    while (1) {
        /* Normal Çalışma Buton Kontrolü */
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
            uint32_t press_start = HAL_GetTick();
            uint32_t last_high_time = press_start;
            uint8_t is_long_press = 0;

            while (1) {
                if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
                    last_high_time = HAL_GetTick(); 
                }
                if ((last_high_time - press_start) >= 3000) {
                    is_long_press = 1;
                    break;
                }
                if ((HAL_GetTick() - last_high_time) > 100) {
                    break;
                }
            }

            if (is_long_press == 1) {
                blink_count = 4; 
                while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET); 
            } 
            else if ((last_high_time - press_start) > 50) { 
                if (blink_count < 7) {
                    blink_count++;
                } else {
                    blink_count = 4;
                }
            } 
            else {
                continue; 
            }

            is_waiting = 0;       
            current_blinks = 0;   
            wait_half_seconds = 0;
            toggle_state = 0; 
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); 
            Flash_Write(blink_count); 
        }
    }
}

/* Donanım Yapılandırmaları */
static void MX_TIM2_Init(void) {
    __HAL_RCC_TIM2_CLK_ENABLE();
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 7999; 
    htim2.Init.Period = 499;    
    HAL_TIM_Base_Init(&htim2);
}

static void MX_GPIO_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void SystemClock_Config(void) {}
void Error_Handler(void) { while(1); }