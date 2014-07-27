#ifndef _BLOCK_H
#define _BLOCK_H

namespace tetris {

enum BlockType {
  NoBlock,
  SBlock,
  ZBlock,
  LineBlock,
  SquareBlock,
  TBlock,
  LBlock,
  RevLBlock
};

class Block {

public:
  Block() { setBlockType(NoBlock); }

  void setRandomBlock();
  void setBlockType(BlockType type);

  BlockType shape() const { return pieceType; }
  int x(int index) const;
  int y(int index) const;
  int minX() const;
  int minY() const;
  int maxX() const;
  int maxY() const;

  Block rotatedLeft() const;
  Block rotatedRight() const;

private:
  int coords[4][2];

  void setX(int index, int x);
  void setY(int index, int y);

  BlockType pieceType;
};

}
#endif // _BLOCK_H
