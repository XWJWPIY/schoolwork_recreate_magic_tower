#ifndef TILE_HPP
#define TILE_HPP

class Tile {
public:
  Tile();
  virtual ~Tile() = default;

  virtual bool isPassable() const;
  void setPassable(bool passable);

  // Getters and Setters
  int getX() const { return x; }
  int getY() const { return y; }
  void setPosition(int new_x, int new_y) {
    x = new_x;
    y = new_y;
  }

protected:
  int x;
  int y;
  bool is_passable = true;
};

#endif // TILE_HPP
