class Tank {
    private:
        std::vector<std::vector<std::wstring>> sprites_normal;
        std::vector<std::vector<std::wstring>> sprites_nivel_dos;
        int direccion = 0;
        bool vivo = true;
    public:
        Tank()
            : sprites_normal{
                {L" ║", L"║█║", L" ━"},   // arriba
                {L" ━ ", L"║█║", L" ║"},  // abajo
                {L"══ ", L"██═", L"══ "}, // derecha
                {L" ══", L"═██", L" ══"}   // izquierda
            }
        {
        	sprites_nivel_dos =
        	{
                {L" ║", L"║█║", L" ━"},   // arriba
                {L" ━ ", L"║█║", L" ║"},  // abajo
                {L"══ ", L"██═", L"══ "}, // derecha
                {L" ══", L"═██", L" ══"}   // izquierda
        	};
        }
        
        void set_direccion(int dir)
        {
            if(dir>=0 && dir <4)
            {
                this->direccion = dir;
            }   	
        }
        std::vector<std::vector<std::wstring>> getSprite_normal(){
            return sprites_normal;
        }
        
        int get_altura() const
        {
            return 3;
        }
        
        int get_ancho() const 
        {
            return 3;
        }

        int get_direccion() {
            return direccion;
        }

};